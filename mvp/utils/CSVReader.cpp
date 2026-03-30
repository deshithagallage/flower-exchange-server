#include "CSVReader.h"
#include "../models/Enums.h"
#include <sstream>

namespace flower_exchange {
using converter::strToInstrument;
using converter::strToSide;
using converter::numericToSide;
using converter::strToExecutionStatus;

std::vector<OrderPtr> CSVReader::readOrders(const std::string& filename) const {
    std::vector<OrderPtr> orders;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    int line_number = 0;
    int order_counter = 0;

    // Read and validate header
    if (!std::getline(file, line)) {
        throw std::runtime_error("File is empty: " + filename);
    }
    line_number++;
    validateHeader(line, 5);  // 5 columns expected: Client Order ID, Instrument, Side, Quantity, Price

    // Read data lines
    while (std::getline(file, line)) {
        line_number++;

        // Skip empty lines
        if (trim(line).empty()) {
            continue;
        }

        // Parse fields with error handling - create orders even with invalid data
        // The validator will later reject them with proper error messages
        auto parts = split(line, ",");

        if (parts.size() != 5) {
            // Pad with empty strings if not enough columns
            while (parts.size() < 5) {
                parts.push_back("");
            }
        }

        // Parse each field with safe defaults for invalid data
        std::string client_order_id = trim(parts[0]);

        // Keep empty client_order_id as is - the validator will reject it
        // If truly empty, use a space so it can be tracked in output
        if (client_order_id.empty()) {
            client_order_id = " ";  // Space character will fail alphanumeric validation
        }

        std::string instrument_str = trim(parts[1]);

        // Safe parse for numeric fields - use invalid values that will be caught by validator
        int side_numeric = 0;
        try {
            side_numeric = stringToInt(trim(parts[2]));
        } catch (...) {
            side_numeric = 0;  // Will map to INVALID side
        }

        int quantity = -1;  // Invalid quantity will be caught by validator
        try {
            quantity = stringToInt(trim(parts[3]));
        } catch (...) {
            quantity = -1;
        }

        double price = -1.0;  // Invalid price will be caught by validator
        try {
            price = stringToDouble(trim(parts[4]));
        } catch (...) {
            price = -1.0;
        }

        // Convert strings to enums (INVALID enum values for bad data)
        Instrument inst = strToInstrument(instrument_str);
        Side side = numericToSide(side_numeric);

        // Generate exchange order ID (ordN format)
        order_counter++;
        std::string exchange_order_id = "ord" + std::to_string(order_counter);

        // Create order with parsed data (even if invalid)
        // The order validator will reject invalid orders later
        auto order = std::make_shared<Order>(
            client_order_id,      // client_id (using client's ID)
            client_order_id,      // client_order_id (same as above)
            inst,
            side,
            price,
            quantity
        );

        // Set the exchange order ID
        order->setExchangeOrderId(exchange_order_id);

        orders.push_back(order);
    }

    file.close();
    return orders;
}

std::vector<ExecutionReportPtr> CSVReader::readExecutionReports(
    const std::string& filename) const {
    
    std::vector<ExecutionReportPtr> reports;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    int line_number = 0;

    // Read and validate header
    if (!std::getline(file, line)) {
        throw std::runtime_error("File is empty: " + filename);
    }
    line_number++;
    validateHeader(line, 7);  // 7 columns expected: Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price

    // Read data lines
    while (std::getline(file, line)) {
        line_number++;

        // Skip empty lines
        if (trim(line).empty()) {
            continue;
        }

        try {
            auto parts = split(line, ",");

            if (parts.size() != 7) {
                throw std::invalid_argument(
                    "Expected 7 columns, got " + std::to_string(parts.size())
                );
            }

            // Parse each field
            std::string order_id = trim(parts[0]);
            std::string client_order_id = trim(parts[1]);
            std::string instrument_str = trim(parts[2]);
            int side_numeric = stringToInt(trim(parts[3]));
            std::string status_str = trim(parts[4]);
            int order_qty = stringToInt(trim(parts[5]));              // Order quantity (from CSV)
            double order_price = stringToDouble(trim(parts[6]));     // Order price (from CSV)

            // Convert strings/numbers to enums
            Instrument inst = strToInstrument(instrument_str);
            Side side = numericToSide(side_numeric);
            
            // Map CSV status names to ExecutionStatus
            ExecutionStatus status;
            std::string upper_status = status_str;
            for (auto& c : upper_status) c = std::toupper(c);
            
            if (upper_status == "NEW") {
                status = ExecutionStatus::QUEUED;
            } else if (upper_status == "FILL") {
                status = ExecutionStatus::FILLED;
            } else if (upper_status == "REJECTED") {
                status = ExecutionStatus::REJECTED;
            } else {
                throw std::invalid_argument("Unknown execution status: " + status_str);
            }

            auto report = std::make_shared<ExecutionReport>(
                order_id, 
                client_order_id,
                inst, 
                side, 
                status,
                order_qty,
                order_price,
                0,              // filled_qty (not in this CSV format)
                0.0,            // execution_price (not in this CSV format)
                ""              // reason is empty when reading from CSV output
            );

            reports.push_back(report);
        } catch (const std::exception& e) {
            throw std::invalid_argument(
                std::string("Line ") + std::to_string(line_number) + 
                ": " + e.what()
            );
        }
    }

    file.close();
    return reports;
}

OrderPtr CSVReader::parseOrderLine(const std::string& line, int line_number) const {
    auto parts = split(line, ",");

    if (parts.size() != 6) {
        throw std::invalid_argument(
            "Expected 6 columns, got " + std::to_string(parts.size())
        );
    }

    // Parse each field
    std::string client_id = trim(parts[0]);
    std::string order_id = trim(parts[1]);
    std::string instrument_str = trim(parts[2]);
    std::string side_str = trim(parts[3]);
    double price = stringToDouble(trim(parts[4]));
    int quantity = stringToInt(trim(parts[5]));

    // Validate non-empty IDs
    if (client_id.empty()) {
        throw std::invalid_argument("Client ID cannot be empty");
    }
    if (order_id.empty()) {
        throw std::invalid_argument("Order ID cannot be empty");
    }

    // Convert strings to enums
    Instrument inst = strToInstrument(instrument_str);
    Side side = strToSide(side_str);

    // Create order (will validate price and quantity)
    return std::make_shared<Order>(client_id, order_id, inst, side, price, quantity);
}

void CSVReader::validateHeader(const std::string& first_line, int expected_columns) const {
    auto parts = split(first_line, ",");

    if (static_cast<int>(parts.size()) != expected_columns) {
        throw std::invalid_argument(
            "Invalid header: expected " + std::to_string(expected_columns) +
            " columns, got " + std::to_string(parts.size())
        );
    }
}

}