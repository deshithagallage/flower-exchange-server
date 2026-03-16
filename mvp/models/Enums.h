#pragma once

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
}

} 