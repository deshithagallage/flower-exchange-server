#pragma once

#include "Order.h"
#include "Enums.h"
#include <string>

namespace flower_exchange {

/**
 * Validator for order submission rules.
 * 
 * Centralizes all validation logic in one place.
 * Easy to modify rules without changing Order class.
 * 
 * Example:
 * @code
 *   OrderValidator validator;
 *   if (!validator.isValid(order)) {
 *       std::cout << "Error: " << validator.getLastError() << "\n";
 *   }
 * @endcode
 */
class OrderValidator {
private:
    std::string last_error_;            // Store last validation error

    // Validation parameters (per requirements)
    static constexpr int MAX_CLIENT_ORDER_ID_LENGTH = 7;  // Max 7 chars
    static constexpr double MIN_PRICE = 0.01;             // Price > 0.0
    static constexpr double MAX_PRICE = 10000.0;
    static constexpr int MIN_QUANTITY = 10;               // Min 10
    static constexpr int MAX_QUANTITY = 1000;             // Max 1000
    static constexpr int QUANTITY_MULTIPLE = 10;          // Must be multiple of 10

public:
    /**
     * Check if order is valid for submission.
     * 
     * Validates:
     * 1. Client Order ID: alphanumeric, max 7 chars
     * 2. Instrument: {Rose, Lavender, Lotus, Tulip, Orchid}
     * 3. Side: 1 (Buy) or 2 (Sell)
     * 4. Price: > 0.0
     * 5. Quantity: multiple of 10, min 10, max 1000
     * 
     * Returns true only if ALL checks pass.
     * If false, call getLastError() to see why.
     * 
     * @param order Order to validate
     * @return true if valid, false otherwise
     */
    bool isValid(const OrderPtr& order);

    const std::string& getLastError() const {
        return last_error_;
    }

    void clearError() {
        last_error_.clear();
    }

private:
    // Individual validation checks (called by isValid)
    bool validateOrderNotNull(const OrderPtr& order);
    bool validateClientOrderId(const OrderPtr& order);    // Renamed from validateOrderId
    bool validatePrice(const OrderPtr& order);
    bool validateQuantity(const OrderPtr& order);
    bool validateInstrument(const OrderPtr& order);
    bool validateSide(const OrderPtr& order);
};

} 