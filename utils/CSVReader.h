#pragma once

#include "../models/Order.h"
#include "EnumConverters.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace flower_exchange {

class CSVReader {
private:
    // Helper function to trim whitespace from strings
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    // Parse a single CSV line
    static OrderPtr parseLine(const std::string& line) {
        std::istringstream iss(line);
        std::string clientId, clientOrderId, instrument, side, priceStr, qtyStr;
        
        // Read all comma-separated fields: ClientID,ClientOrderID,Instrument,Side,Price,Quantity
        if (!std::getline(iss, clientId, ',')) return nullptr;
        if (!std::getline(iss, clientOrderId, ',')) return nullptr;
        if (!std::getline(iss, instrument, ',')) return nullptr;
        if (!std::getline(iss, side, ',')) return nullptr;
        if (!std::getline(iss, priceStr, ',')) return nullptr;
        if (!std::getline(iss, qtyStr, ',')) return nullptr;
        
        // Trim all fields
        clientId = trim(clientId);
        clientOrderId = trim(clientOrderId);
        instrument = trim(instrument);
        side = trim(side);
        priceStr = trim(priceStr);
        qtyStr = trim(qtyStr);
        
        // Skip empty lines
        if (clientOrderId.empty() || instrument.empty()) return nullptr;
        
        try {
            double price = std::stod(priceStr);
            int quantity = std::stoi(qtyStr);
            
            if (price <= 0 || quantity <= 0) return nullptr;
            
            return std::make_shared<Order>(
                clientId,  // Now properly reads the ClientID field
                clientOrderId,
                strToInstrument(instrument),
                strToSide(side),
                price,
                quantity
            );
        } catch (...) {
            return nullptr;  // Parse error
        }
    }

public:
    static std::vector<OrderPtr> readOrdersFromFile(const std::string& filePath) {
        std::vector<OrderPtr> orders;
        std::ifstream file(filePath);
        
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filePath);
        }

        std::string line;
        int lineNum = 0;
        
        // Skip header
        if (std::getline(file, line)) lineNum++;

        while (std::getline(file, line)) {
            lineNum++;
            auto order = parseLine(line);
            if (order) {
                orders.push_back(order);
            }
        }

        file.close();
        return orders;
    }

    static std::vector<OrderPtr> readOrdersFromString(const std::string& csvContent) {
        std::vector<OrderPtr> orders;
        std::istringstream csvStream(csvContent);
        std::string line;
        int lineNum = 0;
        
        // Skip header
        if (std::getline(csvStream, line)) lineNum++;

        while (std::getline(csvStream, line)) {
            lineNum++;
            auto order = parseLine(line);
            if (order) {
                orders.push_back(order);
            }
        }

        return orders;
    }
};

} // namespace flower_exchange
