#pragma once

#include "../models/Order.h"
#include "../models/ExecutionReport.h"
#include "../models/Enums.h"
#include <map>
#include <deque>
#include <vector>
#include <memory>

namespace flower_exchange {

class OrderBook {
private:
    // BUY: highest price first (descending)
    std::map<double, std::deque<OrderPtr>, std::greater<double>> buy_levels_;

    // SELL: lowest price first (ascending)
    std::map<double, std::deque<OrderPtr>> sell_levels_;

    Instrument instrument_;

public:
    explicit OrderBook(Instrument instrument) 
        : instrument_(instrument) {}

    // ==================== MAIN API ====================

    std::vector<ExecutionReportPtr> processOrder(OrderPtr order);

    // ==================== ORDER MANAGEMENT ====================

    void addBuyOrder(const OrderPtr& order);
    void addSellOrder(const OrderPtr& order);

    // ==================== MATCHING ====================

    std::vector<ExecutionReportPtr> processBuyOrder(OrderPtr incoming);
    std::vector<ExecutionReportPtr> processSellOrder(OrderPtr incoming);

    // ==================== QUERIES ====================

    Instrument getInstrument() const { return instrument_; }

    const auto& getBuyLevels() const { return buy_levels_; }
    const auto& getSellLevels() const { return sell_levels_; }

    std::vector<OrderPtr> getAllBuyOrders() const;
    std::vector<OrderPtr> getAllSellOrders() const;

    double getBestBid() const;
    double getBestAsk() const;
    double getSpread() const;

    int getTotalBuyQuantity() const;
    int getTotalSellQuantity() const;

    // ==================== STATE ====================

    void clear();

    bool isEmpty() const {
        return buy_levels_.empty() && sell_levels_.empty();
    }

    int getBuyLevelCount() const { return buy_levels_.size(); }
    int getSellLevelCount() const { return sell_levels_.size(); }

private:
    // ==================== INTERNAL ====================

    int assignPrioritySequenceForPrice(double price, bool is_buy);

    ExecutionReportPtr generateReport(
        const OrderPtr& order,
        ExecutionStatus status,
        const std::string& reason = "",
        double execution_price = -1.0,
        int fill_qty_this_match = -1
    );
};

}