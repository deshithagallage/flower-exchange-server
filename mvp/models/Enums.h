#pragma once

#include <string>
#include <stdexcept>

namespace flower_exchange {

/**
 * Represents the type of flower being traded
 */
enum class Instrument {
    ROSE = 0,
    TULIP = 1,
    LILIES = 2,
    SUNFLOWER = 3,
    DAISY = 4
};

/**
 * Represents the side of an order (buy or sell)
 */
enum class Side {
    BUY = 0,
    SELL = 1
};

/**
 * Represents the current status of an order in the system
 */
enum class OrderStatus {
    PENDING = 0,      // Just created, not yet submitted
    QUEUED = 1,       // In order book, waiting to be matched
    REJECTED = 2,     // Validation failed
    PARTIALLY_FILLED = 3,  // Some quantity matched
    FILLED = 4        // Fully matched
};

/**
 * Represents the execution status of a trade
 */
enum class ExecutionStatus {
    QUEUED = 0,      // Order added to book, no match
    FILLED = 1,      // Order matched, quantity filled
    REJECTED = 2     // Order rejected
};

/**
 * Helper functions for enum conversion
 */
namespace converter {
    /**
     * Convert Instrument enum to string
     * @param inst Instrument enum value
     * @return String representation
     */
    inline const char* instrumentToString(Instrument inst) {
        switch (inst) {
            case Instrument::ROSE:      return "ROSE";
            case Instrument::TULIP:     return "TULIP";
            case Instrument::LILIES:    return "LILIES";
            case Instrument::SUNFLOWER: return "SUNFLOWER";
            case Instrument::DAISY:     return "DAISY";
            default:                    return "UNKNOWN";
        }
    }

    /**
     * Convert Side enum to string
     * @param side Side enum value
     * @return String representation
     */
    inline const char* sideToString(Side side) {
        return side == Side::BUY ? "BUY" : "SELL";
    }

    /**
     * Convert OrderStatus enum to string
     * @param status OrderStatus enum value
     * @return String representation
     */
    inline const char* orderStatusToString(OrderStatus status) {
        switch (status) {
            case OrderStatus::PENDING:           return "PENDING";
            case OrderStatus::QUEUED:            return "QUEUED";
            case OrderStatus::REJECTED:          return "REJECTED";
            case OrderStatus::PARTIALLY_FILLED:  return "PARTIALLY_FILLED";
            case OrderStatus::FILLED:            return "FILLED";
            default:                             return "UNKNOWN";
        }
    }

    /**
     * Convert ExecutionStatus enum to string
     * @param status ExecutionStatus enum value
     * @return String representation
     */
    inline const char* executionStatusToString(ExecutionStatus status) {
        switch (status) {
            case ExecutionStatus::QUEUED:   return "QUEUED";
            case ExecutionStatus::FILLED:   return "FILLED";
            case ExecutionStatus::REJECTED: return "REJECTED";
            default:                        return "UNKNOWN";
        }
    }

    // ==================== STRING TO ENUM CONVERTERS ====================

    /**
     * Convert string to Instrument enum
     * @param str String representation
     * @return Instrument enum value
     * @throws std::invalid_argument if string is not recognized
     */
    inline Instrument strToInstrument(const std::string& str) {
        if (str == "ROSE")      return Instrument::ROSE;
        if (str == "TULIP")     return Instrument::TULIP;
        if (str == "LILIES")    return Instrument::LILIES;
        if (str == "SUNFLOWER") return Instrument::SUNFLOWER;
        if (str == "DAISY")     return Instrument::DAISY;
        throw std::invalid_argument("Unknown instrument: " + str);
    }

    /**
     * Convert string to Side enum
     * @param str String representation
     * @return Side enum value
     * @throws std::invalid_argument if string is not recognized
     */
    inline Side strToSide(const std::string& str) {
        if (str == "BUY")  return Side::BUY;
        if (str == "SELL") return Side::SELL;
        throw std::invalid_argument("Unknown side: " + str);
    }

    /**
     * Convert string to ExecutionStatus enum
     * @param str String representation
     * @return ExecutionStatus enum value
     * @throws std::invalid_argument if string is not recognized
     */
    inline ExecutionStatus strToExecutionStatus(const std::string& str) {
        if (str == "QUEUED")   return ExecutionStatus::QUEUED;
        if (str == "FILLED")   return ExecutionStatus::FILLED;
        if (str == "REJECTED") return ExecutionStatus::REJECTED;
        throw std::invalid_argument("Unknown execution status: " + str);
    }

    /**
     * Convert string to OrderStatus enum
     * @param str String representation
     * @return OrderStatus enum value
     * @throws std::invalid_argument if string is not recognized
     */
    inline OrderStatus strToOrderStatus(const std::string& str) {
        if (str == "PENDING")           return OrderStatus::PENDING;
        if (str == "QUEUED")            return OrderStatus::QUEUED;
        if (str == "REJECTED")          return OrderStatus::REJECTED;
        if (str == "PARTIALLY_FILLED")  return OrderStatus::PARTIALLY_FILLED;
        if (str == "FILLED")            return OrderStatus::FILLED;
        throw std::invalid_argument("Unknown order status: " + str);
    }
}

} 