#include "Order.h"
#include <stdexcept>
#include <sstream>

namespace flower_exchange {

Order::Order(const std::string& client_id,
             const std::string& client_order_id,
             Instrument instrument,
             Side side,
             double price,
             int quantity)
    : client_id_(client_id),
      client_order_id_(client_order_id),
      exchange_order_id_(""),
      instrument_(instrument),
      side_(side),
      price_(price),
      quantity_(quantity),
      remaining_quantity_(quantity),
      priority_sequence_(0),
      status_(OrderStatus::PENDING),
      timestamp_("") {
    
    
    if (client_id.empty()) {
        throw std::invalid_argument("Client ID cannot be empty");
    }
    
    if (client_order_id.empty()) {
        throw std::invalid_argument("Order ID cannot be empty");
    }
    
    if (price <= 0.0) {
        throw std::invalid_argument("Price must be greater than 0");
    }
    
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity must be greater than 0");
    }
}

} 