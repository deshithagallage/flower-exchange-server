#pragma once

#include "../include/order.h"
#include "../include/order_book_side.h"
#include <vector>
#include <memory>

class OrderBook {
private:
    std::string instrument;
    OrderBookSide buyOrders;
    OrderBookSide sellOrders;
    int nextOrderId;
    
    std::string generateOrderId();
    
    // Try to match an incoming order against existing orders
    std::vector<std::shared_ptr<ExecutionReport>> matchOrder(
        std::shared_ptr<Order> incomingOrder,
        const std::string& transactionTime);
    
    // Execute a trade between two orders
    void executeTrade(std::shared_ptr<Order>& aggressiveOrder,
                     std::shared_ptr<Order>& passiveOrder,
                     int& executedQty,
                     double& executedPrice);

public:
    explicit OrderBook(const std::string& instr);
    
    // Process a new incoming order
    std::vector<std::shared_ptr<ExecutionReport>> processOrder(
        std::shared_ptr<Order> order,
        const std::string& transactionTime);
    
    // Get buy side
    OrderBookSide& getBuySide();
    const OrderBookSide& getBuySide() const;
    
    // Get sell side
    OrderBookSide& getSellSide();
    const OrderBookSide& getSellSide() const;
    
    // Get instrument
    const std::string& getInstrument() const;
};
