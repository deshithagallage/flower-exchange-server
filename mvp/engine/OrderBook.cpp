#include "OrderBook.h"
#include <algorithm>
#include <sstream>

namespace flower_exchange {

// ==================== MAIN ENTRY POINT ====================

std::vector<ExecutionReportPtr> OrderBook::processOrder(OrderPtr order) {
    if (!order) {
        throw std::invalid_argument("Order cannot be null");
    }

    // Route to buy or sell processing
    if (order->isBuy()) {
        return processBuyOrder(order);
    } else {
        return processSellOrder(order);
    }
}

// ==================== BUY ORDER MATCHING ====================

std::vector<ExecutionReportPtr> OrderBook::processBuyOrder(OrderPtr incoming) {
    std::vector<ExecutionReportPtr> reports;
    
    incoming->setStatus(OrderStatus::QUEUED);
    int remaining_qty = incoming->getRemainingQuantity();
    int initial_qty = remaining_qty;

    // Iterate through all sell price levels (ascending = best first)
    for (auto it = sell_levels_.begin(); it != sell_levels_.end(); ) {
        double sell_price = it->first;
        std::deque<OrderPtr>& sell_queue = it->second;

        if (sell_price > incoming->getPrice()) {
            break;
        }

        while (!sell_queue.empty() && remaining_qty > 0) {
            OrderPtr sell_order = sell_queue.front();
            int sell_remaining = sell_order->getRemainingQuantity();

            int fill_qty = std::min(remaining_qty, sell_remaining);

            incoming->reduceQuantity(fill_qty);
            sell_order->reduceQuantity(fill_qty);
            remaining_qty -= fill_qty;

            // Determine status based on whether this is partial or full fill
            ExecutionStatus incoming_status = (remaining_qty > 0) ? 
                ExecutionStatus::PARTIAL_FILLED : ExecutionStatus::FILLED;

            // Generate report for the INCOMING BUY order
            auto buy_report = generateReport(
                incoming,
                incoming_status,
                "",
                sell_price,
                fill_qty
            );
            reports.push_back(buy_report);

            // Generate report for the MATCHED SELL order
            // Sell order gets FILLED since it will be removed from queue
            auto sell_report = generateReport(
                sell_order,
                ExecutionStatus::FILLED,
                "",
                sell_price,
                fill_qty
            );
            reports.push_back(sell_report);

            if (sell_order->isFilled()) {
                sell_queue.pop_front();
            }
        }

        if (sell_queue.empty()) {
            it = sell_levels_.erase(it);
        } else {
            ++it;
        }
    }

    if (remaining_qty > 0) {
        incoming->setStatus(OrderStatus::QUEUED);
        incoming->setPrioritySequence(
            assignPrioritySequenceForPrice(incoming->getPrice(), true)
        );
        addBuyOrder(incoming);

        // Only generate QUEUED report if NO partial fills occurred
        // (i.e., order was never matched)
        if (initial_qty == remaining_qty) {
            auto queue_report = generateReport(
                incoming,
                ExecutionStatus::QUEUED,
                "Added to order book"
            );
            reports.push_back(queue_report);
        }
    } else {
        incoming->setStatus(OrderStatus::FILLED);
    }

    return reports;
}

// ==================== SELL ORDER MATCHING ====================

std::vector<ExecutionReportPtr> OrderBook::processSellOrder(OrderPtr incoming) {
    std::vector<ExecutionReportPtr> reports;
    
    incoming->setStatus(OrderStatus::QUEUED);
    int remaining_qty = incoming->getRemainingQuantity();
    int initial_qty = remaining_qty;

    for (auto it = buy_levels_.begin(); it != buy_levels_.end(); ) {
        double buy_price = it->first;
        std::deque<OrderPtr>& buy_queue = it->second;

        if (buy_price < incoming->getPrice()) {
            break;
        }

        while (!buy_queue.empty() && remaining_qty > 0) {
            OrderPtr buy_order = buy_queue.front();
            int buy_remaining = buy_order->getRemainingQuantity();

            int fill_qty = std::min(remaining_qty, buy_remaining);

            incoming->reduceQuantity(fill_qty);
            buy_order->reduceQuantity(fill_qty);
            remaining_qty -= fill_qty;

            // Determine status based on whether this is partial or full fill
            ExecutionStatus incoming_status = (remaining_qty > 0) ? 
                ExecutionStatus::PARTIAL_FILLED : ExecutionStatus::FILLED;

            // Generate report for the INCOMING SELL order
            auto sell_report = generateReport(
                incoming,
                incoming_status,
                "",
                buy_price,
                fill_qty
            );
            reports.push_back(sell_report);

            // Generate report for the MATCHED BUY order
            auto buy_report = generateReport(
                buy_order,
                ExecutionStatus::FILLED,
                "",
                buy_price,
                fill_qty
            );
            reports.push_back(buy_report);

            if (buy_order->isFilled()) {
                buy_queue.pop_front();
            }
        }

        if (buy_queue.empty()) {
            it = buy_levels_.erase(it);
        } else {
            ++it;
        }
    }

    if (remaining_qty > 0) {
        incoming->setStatus(OrderStatus::QUEUED);
        incoming->setPrioritySequence(
            assignPrioritySequenceForPrice(incoming->getPrice(), false)
        );
        addSellOrder(incoming);

        // Only generate QUEUED report if NO partial fills occurred
        // (i.e., order was never matched)
        if (initial_qty == remaining_qty) {
            auto queue_report = generateReport(
                incoming,
                ExecutionStatus::QUEUED,
                "Added to order book"
            );
            reports.push_back(queue_report);
        }
    } else {
        incoming->setStatus(OrderStatus::FILLED);
    }

    return reports;
}

// ==================== ORDER QUEUE MANAGEMENT ====================

void OrderBook::addBuyOrder(const OrderPtr& order) {
    if (!order || order->isSell()) {
        throw std::invalid_argument("Invalid buy order");
    }
    
    double price = order->getPrice();
    buy_levels_[price].push_back(order);
}

void OrderBook::addSellOrder(const OrderPtr& order) {
    if (!order || order->isBuy()) {
        throw std::invalid_argument("Invalid sell order");
    }
    
    double price = order->getPrice();
    sell_levels_[price].push_back(order);
}

// ==================== PRIORITY ASSIGNMENT ====================

int OrderBook::assignPrioritySequenceForPrice(double price, bool is_buy) {
    std::deque<OrderPtr>* queue = nullptr;

    if (is_buy) {
        if (buy_levels_.find(price) != buy_levels_.end()) {
            queue = &buy_levels_[price];
        }
    } else {
        if (sell_levels_.find(price) != sell_levels_.end()) {
            queue = &sell_levels_[price];
        }
    }

    if (queue) {
        return queue->size();
    }
    return 0;
}

// ==================== REPORT GENERATION ====================

ExecutionReportPtr OrderBook::generateReport(
    const OrderPtr& order,
    ExecutionStatus status,
    const std::string& reason,
    double execution_price,
    int fill_qty_this_match) {
    
    auto report = std::make_shared<ExecutionReport>(
        order->getExchangeOrderId(),
        order->getClientOrderId(),
        order->getInstrument(),
        order->getSide(),
        status,
        order->getQuantity(),
        order->getPrice(),
        fill_qty_this_match > 0 ? fill_qty_this_match : 0,
        execution_price > 0 ? execution_price : 0.0,
        reason
    );
    
    return report;
}

// ==================== QUERY METHODS ====================

std::vector<OrderPtr> OrderBook::getAllBuyOrders() const {
    std::vector<OrderPtr> all_orders;
    
    for (const auto& pair : buy_levels_) {
        const auto& queue = pair.second;
        for (const auto& order : queue) {
            all_orders.push_back(order);
        }
    }
    
    return all_orders;
}

std::vector<OrderPtr> OrderBook::getAllSellOrders() const {
    std::vector<OrderPtr> all_orders;
    
    for (const auto& pair : sell_levels_) {
        const auto& queue = pair.second;
        for (const auto& order : queue) {
            all_orders.push_back(order);
        }
    }
    
    return all_orders;
}

double OrderBook::getBestBid() const {
    if (buy_levels_.empty()) return 0.0;
    return buy_levels_.begin()->first;
}

double OrderBook::getBestAsk() const {
    if (sell_levels_.empty()) return 0.0;
    return sell_levels_.begin()->first;
}

double OrderBook::getSpread() const {
    double best_ask = getBestAsk();
    double best_bid = getBestBid();
    
    if (best_ask <= 0.0 || best_bid <= 0.0) {
        return 0.0;
    }
    
    return best_ask - best_bid;
}

int OrderBook::getTotalBuyQuantity() const {
    int total = 0;
    for (const auto& pair : buy_levels_) {
        const auto& queue = pair.second;
        for (const auto& order : queue) {
            total += order->getRemainingQuantity();
        }
    }
    return total;
}

int OrderBook::getTotalSellQuantity() const {
    int total = 0;
    for (const auto& pair : sell_levels_) {
        const auto& queue = pair.second;
        for (const auto& order : queue) {
            total += order->getRemainingQuantity();
        }
    }
    return total;
}

void OrderBook::clear() {
    buy_levels_.clear();
    sell_levels_.clear();
}

} // namespace flower_exchange