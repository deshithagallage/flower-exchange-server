#pragma once

#include "OrderBook.h"
#include "../models/Order.h"
#include "../models/OrderValidator.h"
#include "../models/ExecutionReport.h"
#include "../models/Enums.h"
#include <map>
#include <atomic>
#include <memory>
#include <vector>

namespace flower_exchange {

class Exchange {
private:
    // Order books per instrument
    std::map<Instrument, OrderBook> order_books_;

    // Order validator
    OrderValidator validator_;

    // Thread-safe exchange order ID generator
    std::atomic<int> next_exchange_order_id_{1};

    // Default client ID fallback
    const std::string default_client_id_ = "CLIENT_001";

public:
    Exchange();

    // ==================== MAIN API ====================

    std::vector<ExecutionReportPtr> processOrder(OrderPtr order);

    // ==================== ORDER BOOK ACCESS ====================

    const OrderBook& getOrderBook(Instrument inst) const;
    OrderBook& getOrderBook(Instrument inst);

    // ==================== STATE ====================

    int getOrderBookCount() const { return order_books_.size(); }

    bool isEmpty() const;

    void clear();

    int getTotalOrderCount() const;

    // ==================== QUERIES ====================

    std::vector<OrderPtr> getBuyOrders(Instrument inst) const;
    std::vector<OrderPtr> getSellOrders(Instrument inst) const;

    std::vector<OrderPtr> getAllBuyOrders() const;
    std::vector<OrderPtr> getAllSellOrders() const;

    double getBestBid(Instrument inst) const;
    double getBestAsk(Instrument inst) const;

private:
    // ==================== INTERNAL ====================

    std::string generateExchangeOrderId();

    void assignDefaultClient(OrderPtr& order);
};

}