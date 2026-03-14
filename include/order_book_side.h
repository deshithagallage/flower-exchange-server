#pragma once

#include "../include/order.h"
#include <vector>
#include <memory>

class OrderBookSide {
private:
    std::vector<std::shared_ptr<Order>> orders;
    OrderSide sideType;
    int nextPrioritySequence;

    // Helper to compare orders for sorting
    bool shouldComeBefore(const Order& a, const Order& b) const;

public:
    explicit OrderBookSide(OrderSide type);
    
    // Insert order maintaining price-priority ordering
    void insertOrder(std::shared_ptr<Order> order);
    
    // Remove order by ID
    bool removeOrder(const std::string& orderId);
    
    // Get best bid/ask (front of the list)
    std::shared_ptr<Order> getBestOrder() const;
    
    // Get all orders on this side
    const std::vector<std::shared_ptr<Order>>& getAllOrders() const;
    
    // Check if this side has orders
    bool isEmpty() const;
    
    // Get number of orders
    size_t getOrderCount() const;
    
    // Find order by ID
    std::shared_ptr<Order> findOrder(const std::string& orderId) const;
    
    // Update order quantity
    void updateOrderQuantity(const std::string& orderId, int newQuantity);
};
