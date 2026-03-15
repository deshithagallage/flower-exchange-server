#pragma once
#include "../models/Order.h"
#include "../models/OrderBook.h"
#include "../models/OrderValidator.h"
#include "../models/ExecutionReport.h"
#include <map>
#include <atomic>

namespace flower_exchange {

/**
 * Exchange: Top-level coordinator for order processing.
 * Assigns default client, generates exchange order IDs, validates, and routes to order books.
 */
class Exchange {
private:
    std::map<Instrument, OrderBook> orderBooks_;
    OrderValidator validator_;
    std::atomic<int> nextExchangeOrderId_{1000};
    const std::string defaultClientId_ = "CLIENT_001";

public:
    Exchange() {
        // Initialize order books for all instruments
        orderBooks_.emplace(Instrument::ROSE, OrderBook(Instrument::ROSE));
        orderBooks_.emplace(Instrument::TULIP, OrderBook(Instrument::TULIP));
        orderBooks_.emplace(Instrument::LILIES, OrderBook(Instrument::LILIES));
        orderBooks_.emplace(Instrument::SUNFLOWER, OrderBook(Instrument::SUNFLOWER));
        orderBooks_.emplace(Instrument::DAISY, OrderBook(Instrument::DAISY));
    }

    // Main API
    std::vector<ExecutionReportPtr> processOrder(OrderPtr order);

    // Getters
    const OrderBook& getOrderBook(Instrument inst) const;
    OrderBook& getOrderBook(Instrument inst);

    // State
    void clear();

private:
    std::string generateExchangeOrderId();
    void assignDefaultClient(OrderPtr& order);
};

} // namespace flower_exchange
