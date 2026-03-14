#pragma once

#include "../include/order.h"
#include "../include/order_book.h"
#include <map>
#include <memory>
#include <vector>

class Exchange {
private:
    std::map<std::string, std::shared_ptr<OrderBook>> orderBooks;
    
    // Initialize order books for all flower types
    void initializeOrderBooks();
    
    // Get current transaction time in YYYYMMDD-HHMMSS.sss format
    std::string getCurrentTransactionTime() const;

public:
    Exchange();
    
    // Process an incoming order
    std::vector<std::shared_ptr<ExecutionReport>> processOrder(
        const std::string& clientOrderId,
        const std::string& instrument,
        int side,
        double price,
        int quantity);
    
    // Get order book for a specific instrument
    std::shared_ptr<OrderBook> getOrderBook(const std::string& instrument) const;
    
    // Get all order books
    const std::map<std::string, std::shared_ptr<OrderBook>>& getAllOrderBooks() const;
};
