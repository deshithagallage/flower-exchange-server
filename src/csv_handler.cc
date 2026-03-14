#include "../include/csv_handler.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

std::vector<std::string> CSVHandler::parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(field);  // Add last field
    
    return fields;
}

std::string CSVHandler::escapeCSVField(const std::string& field) {
    if (field.find(',') != std::string::npos || 
        field.find('"') != std::string::npos ||
        field.find('\n') != std::string::npos) {
        std::string escaped = "\"";
        for (char c : field) {
            if (c == '"') {
                escaped += "\"\"";
            } else {
                escaped += c;
            }
        }
        escaped += "\"";
        return escaped;
    }
    return field;
}

std::vector<std::tuple<std::string, std::string, int, double, int>>
CSVHandler::readOrdersFromCSV(const std::string& filename) {
    std::vector<std::tuple<std::string, std::string, int, double, int>> orders;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string line;
    bool isFirstLine = true;
    
    while (std::getline(file, line)) {
        // Skip header line
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        
        if (line.empty()) continue;
        
        auto fields = parseCSVLine(line);
        
        if (fields.size() >= 5) {
            try {
                std::string clientOrderId = fields[0];
                std::string instrument = fields[1];
                int side = std::stoi(fields[2]);
                double price = std::stod(fields[3]);
                int quantity = std::stoi(fields[4]);
                
                orders.emplace_back(clientOrderId, instrument, side, price, quantity);
            } catch (const std::exception& e) {
                // Skip malformed lines
                continue;
            }
        }
    }
    
    file.close();
    return orders;
}

bool CSVHandler::writeExecutionReportsToCSV(
    const std::string& filename,
    const std::vector<std::shared_ptr<ExecutionReport>>& reports) {
    
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "ClientOrderID,OrderID,Instrument,Side,Price,Quantity,Status,Reason,TransactionTime\n";
    
    // Write reports
    for (const auto& report : reports) {
        file << report->clientOrderId << ","
             << report->orderId << ","
             << report->instrument << ","
             << static_cast<int>(report->side) << ","
             << std::fixed << std::setprecision(2) << report->price << ","
             << report->quantity << ","
             << static_cast<int>(report->status) << ",";
        
        if (!report->reason.empty()) {
            file << escapeCSVField(report->reason);
        }
        
        file << "," << report->transactionTime << "\n";
    }
    
    file.close();
    return true;
}
