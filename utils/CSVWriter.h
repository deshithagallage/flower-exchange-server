#pragma once

#include "../models/ExecutionReport.h"
#include <string>
#include <vector>
#include <fstream>

namespace flower_exchange {

class CSVWriter {
public:
    static void writeReportsToFile(const std::string& filePath,
                                   const std::vector<ExecutionReportPtr>& reports) {
        std::ofstream file(filePath, std::ios::app);
        
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filePath);
        }

        // Write header only if file is empty
        file.seekp(0, std::ios::end);
        if (file.tellp() == 0) {
            file << "ClientOrderId,ExchangeOrderId,Instrument,Side,Status,Quantity,Price,Reason,Timestamp\n";
        }

        for (const auto& report : reports) {
            file << report->toCSVRow() << "\n";
        }

        file.close();
    }
};

} // namespace flower_exchange
