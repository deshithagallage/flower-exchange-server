#include "CSVWriter.h"
#include "../models/Enums.h"
#include <iomanip>

namespace flower_exchange {
using converter::instrumentToString;
using converter::sideToString;
using converter::sideToNumeric;
using converter::executionStatusToCSV;
using converter::executionStatusToString;

void CSVWriter::writeOrders(
    const std::vector<OrderPtr>& orders,
    const std::string& filename) const {
    
    std::ofstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    // Write header: Client Order ID, Instrument, Side, Quantity, Price
    writeHeader(file, "Client Order ID,Instrument,Side,Quantity,Price");

    // Write data rows
    for (const auto& order : orders) {
        file << order->getClientOrderId() << ","
             << instrumentToString(order->getInstrument()) << ","
             << sideToNumeric(order->getSide()) << ","
             << order->getQuantity() << ","
             << std::fixed << std::setprecision(2) << order->getPrice() << "\n";
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

    // Write header: Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Transaction Time,Reason
    writeHeader(file, 
        "Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Transaction Time,Reason");

    // Write data rows
    for (const auto& report : reports) {
        // For FILLED/PARTIAL_FILLED: show filled quantity, otherwise show order quantity
        double qty_to_show = report->getOrderQuantity();
        double price_to_show = report->getOrderPrice();
        
        if (report->getStatus() == ExecutionStatus::FILLED || 
            report->getStatus() == ExecutionStatus::PARTIAL_FILLED) {
            qty_to_show = report->getFilledQuantity();
            price_to_show = report->getExecutionPrice();
        }
        
        // Only show reason for rejected orders
        std::string reason = (report->getStatus() == ExecutionStatus::REJECTED) ? 
            report->getReason() : "";
        
        file << report->getExchangeOrderId() << ","
             << report->getClientOrderId() << ","
             << instrumentToString(report->getInstrument()) << ","
             << sideToNumeric(report->getSide()) << ","
             << executionStatusToCSV(report->getStatus()) << ","
             << qty_to_show << ","
             << std::fixed << std::setprecision(2) << price_to_show << ","
             << report->getTimestamp() << ","
             << reason << "\n";
    }

    file.close();
}

void CSVWriter::writeHeader(std::ofstream& file, const std::string& header) const {
    file << header << "\n";
}

} // namespace flower_exchange
