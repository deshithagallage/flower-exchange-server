#include "CSVWriter.h"
#include "../models/Enums.h"
#include <iomanip>

namespace flower_exchange {
using converter::instrumentToString;
using converter::sideToString;
using converter::executionStatusToString;

void CSVWriter::writeOrders(
    const std::vector<OrderPtr>& orders,
    const std::string& filename) const {
    
    std::ofstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    // Write header
    writeHeader(file, "CLIENT_ID,ORDER_ID,INSTRUMENT,SIDE,PRICE,QUANTITY");

    // Write data rows
    for (const auto& order : orders) {
        file << order->getClientId() << ","
             << order->getClientOrderId() << ","
             << instrumentToString(order->getInstrument()) << ","
             << sideToString(order->getSide()) << ","
             << std::fixed << std::setprecision(2) << order->getPrice() << ","
             << order->getQuantity() << "\n";
    }

    file.close();
}

void CSVWriter::writeExecutionReports(
    const std::vector<ExecutionReportPtr>& reports,
    const std::string& filename) const {
    
    std::ofstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    // Write header
    writeHeader(file, 
        "EXCHANGE_ORDER_ID,INSTRUMENT,SIDE,STATUS,FILLED_QUANTITY,EXECUTION_PRICE,REASON");

    // Write data rows
    for (const auto& report : reports) {
        file << report->getExchangeOrderId() << ","
             << instrumentToString(report->getInstrument()) << ","
             << sideToString(report->getSide()) << ","
             << executionStatusToString(report->getStatus()) << ","
             << report->getFilledQuantity() << ","
             << std::fixed << std::setprecision(2) << report->getExecutionPrice() << ","
             << report->getReason() << "\n";
    }

    file.close();
}

void CSVWriter::writeHeader(std::ofstream& file, const std::string& header) const {
    file << header << "\n";
}

} // namespace flower_exchange
