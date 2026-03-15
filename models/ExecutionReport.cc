#include "ExecutionReport.h"
#include <chrono>

namespace flower_exchange {

ExecutionReport::ExecutionReport(const std::string& clientOrderId,
                                 const std::string& exchangeOrderId,
                                 Instrument instrument,
                                 Side side,
                                 ExecutionStatus status,
                                 int quantity,
                                 double price,
                                 const std::string& reason)
    : clientOrderId_(clientOrderId),
      exchangeOrderId_(exchangeOrderId),
      instrument_(instrument),
      side_(side),
      executionStatus_(status),
      quantity_(quantity),
      price_(price),
      reason_(reason) {
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    timestamp_ = std::to_string(time);
}

} // namespace flower_exchange
