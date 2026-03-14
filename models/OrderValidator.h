#pragma once

#include "Order.h"
#include <string>

namespace flower_exchange {

class OrderValidator {
public:
    bool validate(const Order& order, std::string& rejectReason) const {
        // Check client ID
        if (order.getClientId().empty()) {
            rejectReason = "Client ID cannot be empty";
            return false;
        }

        // Check client order ID
        if (order.getClientOrderId().empty()) {
            rejectReason = "Client order ID cannot be empty";
            return false;
        }

        // Check instrument
        if (order.getInstrument() == Instrument::UNKNOWN) {
            rejectReason = "Invalid instrument";
            return false;
        }

        // Check price
        if (order.getPrice() <= 0.0) {
            rejectReason = "Price must be positive";
            return false;
        }

        // Check quantity
        if (order.getQuantity() <= 0) {
            rejectReason = "Quantity must be positive";
            return false;
        }

        return true;
    }
};

} // namespace flower_exchange
