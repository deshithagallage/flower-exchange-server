#include "Order.h"
#include <chrono>

namespace flower_exchange {

Order::Order(const std::string& clientId,
             const std::string& clientOrderId,
             Instrument instrument,
             Side side,
             double price,
             int quantity)
    : clientId_(clientId),
      clientOrderId_(clientOrderId),
      instrument_(instrument),
      side_(side),
      price_(price),
      quantity_(quantity),
      remainingQuantity_(quantity),
      prioritySequence_(0),
      status_(OrderStatus::NEW) {
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    timestamp_ = std::to_string(time);
}

} // namespace flower_exchange
