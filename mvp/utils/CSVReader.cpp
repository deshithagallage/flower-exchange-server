#include "CSVReader.h"
#include "../models/Enums.h"
#include <sstream>

namespace flower_exchange {
using converter::strToInstrument;
using converter::strToSide;
using converter::strToExecutionStatus;

std::vector<OrderPtr> CSVReader::readOrders(const std::string& filename) const {
    std::vector<OrderPtr> orders;
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
    validateHeader(line, 6);  // 6 columns expected

    // Read data lines
    while (std::getline(file, line)) {
        line_number++;

        // Skip empty lines
        if (trim(line).empty()) {
            continue;
        }

        try {
            auto order = parseOrderLine(line, line_number);
            orders.push_back(order);
        } catch (const std::exception& e) {
            throw std::invalid_argument(
                std::string("Line ") + std::to_string(line_number) + 
                ": " + e.what()
            );
        }
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
    validateHeader(line, 7);  // 7 columns expected

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
            std::string exchange_id = trim(parts[0]);
            std::string instrument_str = trim(parts[1]);
            std::string side_str = trim(parts[2]);
            std::string status_str = trim(parts[3]);
            int filled_qty = stringToInt(trim(parts[4]));
            double execution_price = stringToDouble(trim(parts[5]));
            std::string reason = trim(parts[6]);

            // Convert strings to enums
            Instrument inst = strToInstrument(instrument_str);
            Side side = strToSide(side_str);
            ExecutionStatus status = strToExecutionStatus(status_str);

            auto report = std::make_shared<ExecutionReport>(
                exchange_id, inst, side, status,
                filled_qty, execution_price, reason
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