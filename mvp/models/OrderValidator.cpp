#include "OrderValidator.h"
#include <sstream>

namespace flower_exchange {

bool OrderValidator::isValid(const OrderPtr& order) {
    clearError();

    // Run all validation checks in sequence
    if (!validateOrderNotNull(order)) return false;
    if (!validateClientId(order)) return false;
    if (!validateOrderId(order)) return false;
    if (!validatePrice(order)) return false;
    if (!validateQuantity(order)) return false;
    if (!validateInstrument(order)) return false;

    return true;  // All checks passed
}

bool OrderValidator::validateOrderNotNull(const OrderPtr& order) {
    if (!order) {
        last_error_ = "Order cannot be null";
        return false;
    }
    return true;
}

bool OrderValidator::validateClientId(const OrderPtr& order) {
    if (order->getClientId().empty()) {
        last_error_ = "Client ID cannot be empty";
        return false;
    }
    if (order->getClientId().length() > 100) {
        last_error_ = "Client ID too long (max 100 chars)";
        return false;
    }
    return true;
}

bool OrderValidator::validateOrderId(const OrderPtr& order) {
    if (order->getClientOrderId().empty()) {
        last_error_ = "Order ID cannot be empty";
        return false;
    }
    if (order->getClientOrderId().length() > 100) {
        last_error_ = "Order ID too long (max 100 chars)";
        return false;
    }
    return true;
}

bool OrderValidator::validatePrice(const OrderPtr& order) {
    double price = order->getPrice();

    if (price < MIN_PRICE) {
        std::ostringstream oss;
        oss << "Price too low (min " << MIN_PRICE << ")";
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

    if (qty < MIN_QUANTITY) {
        std::ostringstream oss;
        oss << "Quantity too low (min " << MIN_QUANTITY << ")";
        last_error_ = oss.str();
        return false;
    }

    if (qty > MAX_QUANTITY) {
        std::ostringstream oss;
        oss << "Quantity too high (max " << MAX_QUANTITY << ")";
        last_error_ = oss.str();
        return false;
    }

    return true;
}

bool OrderValidator::validateInstrument(const OrderPtr& order) {
    return true;
}

}