#include "OrderValidator.h"
#include <sstream>
#include <cctype>
#include <algorithm>

namespace flower_exchange {

bool OrderValidator::isValid(const OrderPtr& order) {
    clearError();

    // Run all validation checks in sequence
    if (!validateOrderNotNull(order)) return false;
    if (!validateClientOrderId(order)) return false;  // Renamed method
    if (!validateInstrument(order)) return false;
    if (!validateSide(order)) return false;           // New validation
    if (!validatePrice(order)) return false;
    if (!validateQuantity(order)) return false;

    return true;  // All checks passed
}

bool OrderValidator::validateOrderNotNull(const OrderPtr& order) {
    if (!order) {
        last_error_ = "Order cannot be null";
        return false;
    }
    return true;
}

bool OrderValidator::validateClientOrderId(const OrderPtr& order) {
    std::string client_order_id = order->getClientOrderId();
    
    // Check if empty
    if (client_order_id.empty()) {
        last_error_ = "Invalid client order id";
        return false;
    }
    
    // Check max 7 characters
    if (client_order_id.length() > MAX_CLIENT_ORDER_ID_LENGTH) {
        last_error_ = "Invalid client order id";
        return false;
    }
    
    // Check alphanumeric only
    for (char c : client_order_id) {
        if (!std::isalnum(c)) {
            last_error_ = "Invalid client order id";
            return false;
        }
    }
    
    return true;
}

bool OrderValidator::validatePrice(const OrderPtr& order) {
    double price = order->getPrice();

    if (price <= 0.0) {
        last_error_ = "Invalid price";
        return false;
    }

    if (price > MAX_PRICE) {
        last_error_ = "Invalid price";
        return false;
    }

    return true;
}

bool OrderValidator::validateQuantity(const OrderPtr& order) {
    int qty = order->getQuantity();

    // Check minimum
    if (qty < MIN_QUANTITY) {
        last_error_ = "Invalid size";
        return false;
    }

    // Check maximum
    if (qty > MAX_QUANTITY) {
        last_error_ = "Invalid size";
        return false;
    }

    // Check multiple of 10
    if (qty % QUANTITY_MULTIPLE != 0) {
        last_error_ = "Invalid size";
        return false;
    }

    return true;
}

bool OrderValidator::validateInstrument(const OrderPtr& order) {
    // All instruments in the enum are valid
    // The enum conversion already validates it
    Instrument inst = order->getInstrument();
    
    // Just verify it's one of the allowed values
    switch (inst) {
        case Instrument::ROSE:
        case Instrument::LAVENDER:
        case Instrument::LOTUS:
        case Instrument::TULIP:
        case Instrument::ORCHID:
            return true;
        default:
            last_error_ = "Invalid instrument";
            return false;
    }
}

bool OrderValidator::validateSide(const OrderPtr& order) {
    Side side = order->getSide();
    
    // Valid sides are BUY (1) and SELL (2)
    switch (side) {
        case Side::BUY:   // 1
        case Side::SELL:  // 2
            return true;
        default:
            last_error_ = "Invalid side";
            return false;
    }
}

}