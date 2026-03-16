#pragma once

#include "Enums.h"
#include <string>
#include <memory>

namespace flower_exchange {

/**
 * Represents a trading order in the exchange.
 * 
 * Immutable after creation (except for status updates during lifecycle).
 * Handles quantity reduction as trades are executed.
 * 
 * Thread-safe: Each thread operates on different Order instances.
 */
class Order {
private:
    std::string client_id_;             
    std::string client_order_id_;       
    std::string exchange_order_id_;     
    Instrument instrument_;            
    Side side_;                         
    double price_;                      
    int quantity_;                      
    int remaining_quantity_;            
    int priority_sequence_;            
    OrderStatus status_;                
    std::string timestamp_;             

public:
    /**
     * Constructor: Create a new order.
     * 
     * @param client_id Client identifier
     * @param client_order_id Client's order reference
     * @param instrument Which flower
     * @param side BUY or SELL
     * @param price Per-unit price
     * @param quantity Total quantity
     * 
     * @throws std::invalid_argument if parameters are invalid
     * 
     * Example:
     * @code
     *   auto order = std::make_shared<Order>(
     *       "CLIENT_001",      // Who
     *       "ORD_123",         // Their reference
     *       Instrument::ROSE,  // What
     *       Side::BUY,         // Buy or sell
     *       100.50,            // Price per unit
     *       10                 // Quantity
     *   );
     * @endcode
     */
    Order(const std::string& client_id,
          const std::string& client_order_id,
          Instrument instrument,
          Side side,
          double price,
          int quantity);


    const std::string& getClientId() const { return client_id_; }

    const std::string& getClientOrderId() const { return client_order_id_; }

    const std::string& getExchangeOrderId() const { return exchange_order_id_; }

    Instrument getInstrument() const { return instrument_; }

    Side getSide() const { return side_; }

    double getPrice() const { return price_; }

    int getQuantity() const { return quantity_; }

    int getRemainingQuantity() const { return remaining_quantity_; }

    int getPrioritySequence() const { return priority_sequence_; }

    OrderStatus getStatus() const { return status_; }

    const std::string& getTimestamp() const { return timestamp_; }

    void setExchangeOrderId(const std::string& id) {
        exchange_order_id_ = id;
    }

    void setPrioritySequence(int seq) {
        priority_sequence_ = seq;
    }

    void setStatus(OrderStatus status) {
        status_ = status;
    }

    void setTimestamp(const std::string& ts) {
        timestamp_ = ts;
    }

    // ==================== BUSINESS LOGIC METHODS ====================

    bool isBuy() const { return side_ == Side::BUY; }

    bool isSell() const { return side_ == Side::SELL; }

    void reduceQuantity(int qty) {
        remaining_quantity_ -= qty;
    }

    bool isFilled() const {
        return remaining_quantity_ <= 0;
    }

    double getFillPercentage() const {
        if (quantity_ == 0) return 0.0;
        int filled = quantity_ - remaining_quantity_;
        return (static_cast<double>(filled) / quantity_) * 100.0;
    }

    int getFilledQuantity() const {
        return quantity_ - remaining_quantity_;
    }
};

/**
 * Smart pointer type for Order.
 * Use this everywhere instead of Order*
 * 
 * Example:
 * @code
 *   OrderPtr order = std::make_shared<Order>(...);
 *   // Automatically deleted when last reference is destroyed
 * @endcode
 */
using OrderPtr = std::shared_ptr<Order>;

} 