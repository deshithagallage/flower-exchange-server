#pragma once

#include "Order.h"
#include <string>
#include <vector>

namespace flower_exchange {

class OrderValidator {
public:
    bool validate(const Order& order, std::string& rejectReasons) const {
        std::vector<std::string> reasons;
        bool isValid = true;

        // Check client ID
        if (order.getClientId().empty()) {
            reasons.push_back("Client ID cannot be empty");
            isValid = false;
        }

        // Check client order ID
        if (order.getClientOrderId().empty()) {
            reasons.push_back("Client order ID cannot be empty");
            isValid = false;
        }

        // Check instrument
        if (order.getInstrument() == Instrument::UNKNOWN) {
            reasons.push_back("Invalid instrument");
            isValid = false;
        }

        // Check side - must be BUY or SELL
        if (order.getSide() == Side::UNKNOWN) {
            reasons.push_back("Invalid side - must be BUY or SELL");
            isValid = false;
        }

        // Check price - must be >= 0
        if (order.getPrice() < 0.0) {
            reasons.push_back("Invalid price - must be greater than or equal to 0");
            isValid = false;
        }

        // Check quantity - must be between 10 and 1000
        if (order.getQuantity() < 10 || order.getQuantity() > 1000) {
            reasons.push_back("Invalid size - must be between 10 and 1000");
            isValid = false;
        }

        // Check quantity - must be a multiple of 10
        if (order.getQuantity() > 0 && order.getQuantity() % 10 != 0) {
            reasons.push_back("Invalid size - must be a multiple of 10");
            isValid = false;
        }

        // Combine all reasons with semicolon separator
        if (!isValid) {
            rejectReasons.clear();
            for (size_t i = 0; i < reasons.size(); ++i) {
                rejectReasons += reasons[i];
                if (i < reasons.size() - 1) {
                    rejectReasons += "; ";
                }
            }
        }

        return isValid;
    }
};

} // namespace flower_exchange
