#include "../include/order_book_side.h"
#include <algorithm>

OrderBookSide::OrderBookSide(OrderSide type) 
    : sideType(type), nextPrioritySequence(0) {
}

bool OrderBookSide::shouldComeBefore(const Order& a, const Order& b) const {
    // For BUY side: higher prices first (descending), then by priority
    // For SELL side: lower prices first (ascending), then by priority
    if (sideType == OrderSide::BUY) {
        if (a.price != b.price) {
            return a.price > b.price;  // Higher price first
        }
    } else {
        if (a.price != b.price) {
            return a.price < b.price;  // Lower price first
        }
    }
    // Same price: earlier priority comes first
    return a.prioritySequence < b.prioritySequence;
}

void OrderBookSide::insertOrder(std::shared_ptr<Order> order) {
    order->prioritySequence = nextPrioritySequence++;
    
    // Find insertion position
    auto it = std::lower_bound(orders.begin(), orders.end(), order,
        [this](const std::shared_ptr<Order>& a, const std::shared_ptr<Order>& b) {
            return !shouldComeBefore(*b, *a);
        });
    
    orders.insert(it, order);
}

bool OrderBookSide::removeOrder(const std::string& orderId) {
    auto it = std::find_if(orders.begin(), orders.end(),
        [&orderId](const std::shared_ptr<Order>& o) {
            return o->orderId == orderId;
        });
    
    if (it != orders.end()) {
        orders.erase(it);
        return true;
    }
    return false;
}

std::shared_ptr<Order> OrderBookSide::getBestOrder() const {
    if (orders.empty()) {
        return nullptr;
    }
    return orders.front();
}

const std::vector<std::shared_ptr<Order>>& OrderBookSide::getAllOrders() const {
    return orders;
}

bool OrderBookSide::isEmpty() const {
    return orders.empty();
}

size_t OrderBookSide::getOrderCount() const {
    return orders.size();
}

std::shared_ptr<Order> OrderBookSide::findOrder(const std::string& orderId) const {
    auto it = std::find_if(orders.begin(), orders.end(),
        [&orderId](const std::shared_ptr<Order>& o) {
            return o->orderId == orderId;
        });
    
    if (it != orders.end()) {
        return *it;
    }
    return nullptr;
}

void OrderBookSide::updateOrderQuantity(const std::string& orderId, int newQuantity) {
    auto order = findOrder(orderId);
    if (order) {
        order->quantity = newQuantity;
    }
}
