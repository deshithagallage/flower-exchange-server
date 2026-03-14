#pragma once

#include "../include/order.h"
#include <vector>
#include <string>
#include <memory>

class CSVHandler {
public:
    // Read orders from CSV file
    static std::vector<std::tuple<std::string, std::string, int, double, int>>
    readOrdersFromCSV(const std::string& filename);
    
    // Write execution reports to CSV file
    static bool writeExecutionReportsToCSV(
        const std::string& filename,
        const std::vector<std::shared_ptr<ExecutionReport>>& reports);
    
private:
    // Parse a CSV line (simple implementation)
    static std::vector<std::string> parseCSVLine(const std::string& line);
    
    // Escape CSV field if needed
    static std::string escapeCSVField(const std::string& field);
};
