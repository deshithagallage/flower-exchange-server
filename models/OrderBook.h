#pragma once

#include "Order.h"
#include "ExecutionReport.h"
#include <map>
#include <deque>
#include <vector>
#include <memory>

namespace flower_exchange {

/**
 * Order book with price-level buckets using FIFO within each price level.
 * BUY orders: prices sorted descending (best buy first)
 * SELL orders: prices sorted ascending (best sell first)
 * Within each price level: FIFO queue using deque
 */
class OrderBook {
private:
    // Price buckets with FIFO queues
    // BUY: map<double, deque> with prices in descending order
    std::map<double, std::deque<OrderPtr>, std::greater<double>> buyLevels_;
    // SELL: map<double, deque> with prices in ascending order
    std::map<double, std::deque<OrderPtr>> sellLevels_;
    Instrument instrument_;

public:
    OrderBook(Instrument instrument) : instrument_(instrument) {}

    // Add orders to appropriate price bucket
    void addBuyOrder(const OrderPtr& order);
    void addSellOrder(const OrderPtr& order);

    // Main matching processor
    std::vector<ExecutionReportPtr> processOrder(OrderPtr order);

    // Internal matching methods
    std::vector<ExecutionReportPtr> processBuyOrder(OrderPtr incoming);
    std::vector<ExecutionReportPtr> processSellOrder(OrderPtr incoming);

    // Getters
    Instrument getInstrument() const { return instrument_; }
    const auto& getBuyLevels() const { return buyLevels_; }
    const auto& getSellLevels() const { return sellLevels_; }

    std::vector<OrderPtr> getAllBuyOrders() const;
    std::vector<OrderPtr> getAllSellOrders() const;

    // State management
    void clear();
    bool isEmpty() const { return buyLevels_.empty() && sellLevels_.empty(); }

private:
    int assignPrioritySequenceForPrice(double price, bool isBuy);
    ExecutionReportPtr generateReport(const OrderPtr& order,
                                     ExecutionStatus status,
                                     const std::string& reason = "",
                                     double executionPrice = -1.0,
                                     int fillQtyThisMatch = -1);
};

} // namespace flower_exchange
