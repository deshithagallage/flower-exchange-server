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
        last_error_ = "Client Order ID cannot be empty";
        return false;
    }
    
    // Check max 7 characters
    if (client_order_id.length() > MAX_CLIENT_ORDER_ID_LENGTH) {
        std::ostringstream oss;
        oss << "Client Order ID too long (max " << MAX_CLIENT_ORDER_ID_LENGTH 
            << " chars, got " << client_order_id.length() << ")";
        last_error_ = oss.str();
        return false;
    }
    
    // Check alphanumeric only
    for (char c : client_order_id) {
        if (!std::isalnum(c)) {
            last_error_ = "Client Order ID must be alphanumeric (letters and digits only)";
            return false;
        }
    }
    
    return true;
}

bool OrderValidator::validatePrice(const OrderPtr& order) {
    double price = order->getPrice();

    if (price <= 0.0) {
        std::ostringstream oss;
        oss << "Price must be greater than 0.0 (got " << price << ")";
        last_error_ = oss.str();
        return false;
    }

    if (price > MAX_PRICE) {
        std::ostringstream oss;
        oss << "Price too high (max " << MAX_PRICE << ")";
        last_error_ = oss.str();
        return false;
    }

    return true;
}

bool OrderValidator::validateQuantity(const OrderPtr& order) {
    int qty = order->getQuantity();

    // Check minimum
    if (qty < MIN_QUANTITY) {
        std::ostringstream oss;
        oss << "Quantity too low (min " << MIN_QUANTITY << ", got " << qty << ")";
        last_error_ = oss.str();
        return false;
    }

    // Check maximum
    if (qty > MAX_QUANTITY) {
        std::ostringstream oss;
        oss << "Quantity too high (max " << MAX_QUANTITY << ", got " << qty << ")";
        last_error_ = oss.str();
        return false;
    }

    // Check multiple of 10
    if (qty % QUANTITY_MULTIPLE != 0) {
        std::ostringstream oss;
        oss << "Quantity must be a multiple of 10 (got " << qty << ")";
        last_error_ = oss.str();
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
            last_error_ = "Invalid side (must be 1=BUY or 2=SELL)";
            return false;
    }
}

}