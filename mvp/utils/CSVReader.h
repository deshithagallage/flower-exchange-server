#pragma once

#include "../models/Order.h"
#include "../models/ExecutionReport.h"
#include "StringUtils.h"
#include <fstream>
#include <vector>
#include <memory>
#include <stdexcept>

namespace flower_exchange {

/** CSVReader: Parse CSV files into Order/ExecutionReport objects */

// Read orders from CSV, e.g., "CLIENT_001,ORD_001,ROSE,BUY,100.50,10"
class CSVReader {
public:
    std::vector<OrderPtr> readOrders(const std::string& filename) const; // returns vector of orders

    std::vector<ExecutionReportPtr> readExecutionReports(const std::string& filename) const; // returns vector of reports

private:
    OrderPtr parseOrderLine(const std::string& line, int line_number) const; // parse single CSV order line, e.g., "CLIENT_001,ORD_001,ROSE,BUY,100.50,10"

    void validateHeader(const std::string& first_line, int expected_columns) const; // check CSV header, e.g., "CLIENT_ID,ORDER_ID,INSTRUMENT,SIDE,PRICE,QUANTITY"
};

} // namespace flower_exchange