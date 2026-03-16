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

    // Validation parameters (configurable)
    static constexpr double MIN_PRICE = 0.01;
    static constexpr double MAX_PRICE = 10000.0;
    static constexpr int MIN_QUANTITY = 1;
    static constexpr int MAX_QUANTITY = 1000;

public:
    /**
     * Check if order is valid for submission.
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
    bool validateClientId(const OrderPtr& order);
    bool validateOrderId(const OrderPtr& order);
    bool validatePrice(const OrderPtr& order);
    bool validateQuantity(const OrderPtr& order);
    bool validateInstrument(const OrderPtr& order);
};

} 