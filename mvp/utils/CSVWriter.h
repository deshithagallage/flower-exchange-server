#pragma once

#include "../models/Order.h"
#include "../models/ExecutionReport.h"
#include "StringUtils.h"
#include <fstream>
#include <vector>
#include <memory>

namespace flower_exchange {

// Write orders to CSV file, e.g., writeOrders(orders, "orders.csv")
class CSVWriter {
public:
    // Write orders to CSV file with headers CLIENT_ID,ORDER_ID,INSTRUMENT,SIDE,PRICE,QUANTITY
    void writeOrders(const std::vector<OrderPtr>& orders, const std::string& filename) const;

    // Write execution reports to CSV file with headers EXCHANGE_ORDER_ID,INSTRUMENT,SIDE,STATUS,FILLED_QUANTITY,EXECUTION_PRICE,REASON
    void writeExecutionReports(const std::vector<ExecutionReportPtr>& reports, const std::string& filename) const;

private:
    // Write a single header row to the CSV file
    void writeHeader(std::ofstream& file, const std::string& header) const;
};

} // namespace flower_exchange