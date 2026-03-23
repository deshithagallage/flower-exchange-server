#pragma once

#include <string>
#include <stdexcept>
#include <cctype>

namespace flower_exchange {

/**
 * Represents the type of flower being traded
 * Valid instruments: Rose, Lavender, Lotus, Tulip, Orchid
 */
enum class Instrument {
    ROSE = 0,
    LAVENDER = 1,
    LOTUS = 2,
    TULIP = 3,
    ORCHID = 4
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
    QUEUED = 0,           // Order added to book, no match
    FILLED = 1,           // Order matched, quantity filled completely
    REJECTED = 2,         // Order rejected
    PARTIAL_FILLED = 3    // Order partially filled (some quantity matched, remainder in book)
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
            case Instrument::ROSE:      return "Rose";
            case Instrument::LAVENDER:  return "Lavender";
            case Instrument::LOTUS:     return "Lotus";
            case Instrument::TULIP:     return "Tulip";
            case Instrument::ORCHID:    return "Orchid";
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
     * Convert Side enum to numeric value (1=BUY, 2=SELL)
     * @param side Side enum value
     * @return Numeric representation
     */
    inline int sideToNumeric(Side side) {
        return side == Side::BUY ? 1 : 2;
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

    /**
     * Convert ExecutionStatus enum to short CSV format (New, Fill, Rejected, Pfill)
     * @param status ExecutionStatus enum value
     * @return Short string representation
     */
    inline const char* executionStatusToCSV(ExecutionStatus status) {
        switch (status) {
            case ExecutionStatus::QUEUED:           return "New";        // Added to order book
            case ExecutionStatus::FILLED:           return "Fill";       // Fully matched
            case ExecutionStatus::PARTIAL_FILLED:   return "Pfill";      // Partially matched
            case ExecutionStatus::REJECTED:         return "Rejected";   // Validation failed
            default:                                return "UNKNOWN";
        }
    }

    // ==================== STRING TO ENUM CONVERTERS ====================

    /**
     * Convert string to Instrument enum (case-insensitive)
     * @param str String representation (e.g., "Rose", "ROSE", "rose")
     * @return Instrument enum value
     * @throws std::invalid_argument if string is not recognized
     */
    inline Instrument strToInstrument(const std::string& str) {
        // Case-insensitive comparison
        std::string lower_str = str;
        for (auto& c : lower_str) c = std::tolower(c);
        
        if (lower_str == "rose")      return Instrument::ROSE;
        if (lower_str == "lavender")  return Instrument::LAVENDER;
        if (lower_str == "lotus")     return Instrument::LOTUS;
        if (lower_str == "tulip")     return Instrument::TULIP;
        if (lower_str == "orchid")    return Instrument::ORCHID;
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
     * Convert numeric value to Side enum (1=BUY, 2=SELL)
     * @param num Numeric side value
     * @return Side enum value
     * @throws std::invalid_argument if num is not 1 or 2
     */
    inline Side numericToSide(int num) {
        if (num == 1) return Side::BUY;
        if (num == 2) return Side::SELL;
        throw std::invalid_argument("Side must be 1 (BUY) or 2 (SELL), got " + std::to_string(num));
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