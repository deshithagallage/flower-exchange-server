#include "ExecutionReport.h"
#include <stdexcept>

namespace flower_exchange {

ExecutionReport::ExecutionReport(
    const std::string& exchange_order_id,
    Instrument instrument,
    Side side,
    ExecutionStatus status,
    int filled_quantity,
    double execution_price,
    const std::string& reason,
    const std::string& timestamp)
    : exchange_order_id_(exchange_order_id),
      timestamp_(timestamp),
      status_(status),
      filled_quantity_(filled_quantity),
      execution_price_(execution_price),
      reason_(reason),
      instrument_(instrument),
      side_(side) {
    
    if (exchange_order_id.empty()) {
        throw std::invalid_argument("Exchange order ID cannot be empty");
    }

    if (filled_quantity < 0) {
        throw std::invalid_argument("Filled quantity cannot be negative");
    }

    if (execution_price < 0.0) {
        throw std::invalid_argument("Execution price cannot be negative");
    }

    if (status == ExecutionStatus::FILLED && execution_price <= 0.0) {
        throw std::invalid_argument("Execution price required for filled orders");
    }

    if (status == ExecutionStatus::REJECTED && reason.empty()) {
        throw std::invalid_argument("Rejection reason required");
    }
}

} 