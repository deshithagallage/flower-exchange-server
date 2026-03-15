#include "OrderBook.h"
#include <algorithm>

namespace flower_exchange {

void OrderBook::addBuyOrder(const OrderPtr& order) {
    int seq = assignPrioritySequenceForPrice(order->getPrice(), true);
    order->setPrioritySequence(seq);
    order->setStatus(OrderStatus::NEW);
    buyLevels_[order->getPrice()].push_back(order);
}

void OrderBook::addSellOrder(const OrderPtr& order) {
    int seq = assignPrioritySequenceForPrice(order->getPrice(), false);
    order->setPrioritySequence(seq);
    order->setStatus(OrderStatus::NEW);
    sellLevels_[order->getPrice()].push_back(order);
}

int OrderBook::assignPrioritySequenceForPrice(double price, bool isBuy) {
    if (isBuy) {
        if (buyLevels_.find(price) != buyLevels_.end()) {
            return buyLevels_[price].size() + 1;
        }
    } else {
        if (sellLevels_.find(price) != sellLevels_.end()) {
            return sellLevels_[price].size() + 1;
        }
    }
    return 1;  // First order at this price level
}

std::vector<ExecutionReportPtr> OrderBook::processOrder(OrderPtr order) {
    if (order->isBuy()) {
        return processBuyOrder(order);
    } else {
        return processSellOrder(order);
    }
}

std::vector<ExecutionReportPtr> OrderBook::processBuyOrder(OrderPtr incoming) {
    std::vector<ExecutionReportPtr> reports;

    // Match against sell orders from lowest price
    while (!incoming->isFilled() && !sellLevels_.empty()) {
        auto bestSellLevel = sellLevels_.begin();
        double bestSellPrice = bestSellLevel->first;

        // Check if buy price can match sell price
        if (incoming->getPrice() < bestSellPrice) {
            break;  // No match possible
        }

        // Match with front order (FIFO within price level)
        auto& sellQueue = bestSellLevel->second;
        auto matchedOrder = sellQueue.front();

        int matchQty = std::min(incoming->getRemainingQuantity(), 
                               matchedOrder->getRemainingQuantity());

        incoming->reduceQuantity(matchQty);
        matchedOrder->reduceQuantity(matchQty);

        // Generate report for incoming order FIRST after this match (use matched order's price)
        if (incoming->isFilled()) {
            incoming->setStatus(OrderStatus::FILLED);
            reports.push_back(generateReport(incoming, ExecutionStatus::FILL, "", matchedOrder->getPrice(), matchQty));
        } else if (incoming->getRemainingQuantity() < incoming->getQuantity()) {
            // Partially filled after this match
            incoming->setStatus(OrderStatus::PARTIALLY_FILLED);
            reports.push_back(generateReport(incoming, ExecutionStatus::PFILL, "", matchedOrder->getPrice(), matchQty));
        }

        // Then generate report for matched sell order (use its own price)
        if (matchedOrder->isFilled()) {
            matchedOrder->setStatus(OrderStatus::FILLED);
            reports.push_back(generateReport(matchedOrder, ExecutionStatus::FILL, "", -1.0, matchQty));
            sellQueue.pop_front();
        } else {
            matchedOrder->setStatus(OrderStatus::PARTIALLY_FILLED);
            reports.push_back(generateReport(matchedOrder, ExecutionStatus::PFILL, "", -1.0, matchQty));
        }

        // Remove empty price level
        if (sellQueue.empty()) {
            sellLevels_.erase(bestSellLevel);
        }
    }

    // Handle case where incoming order had NO matches at all
    if (incoming->getRemainingQuantity() == incoming->getQuantity()) {
        addBuyOrder(incoming);
        reports.push_back(generateReport(incoming, ExecutionStatus::NEW));
    } else if (!incoming->isFilled()) {
        // Had some matches but not fully filled - add to order book
        addBuyOrder(incoming);
    }

    return reports;
}

std::vector<ExecutionReportPtr> OrderBook::processSellOrder(OrderPtr incoming) {
    std::vector<ExecutionReportPtr> reports;

    // Match against buy orders from highest price
    while (!incoming->isFilled() && !buyLevels_.empty()) {
        auto bestBuyLevel = buyLevels_.begin();
        double bestBuyPrice = bestBuyLevel->first;

        // Check if sell price can match buy price
        if (incoming->getPrice() > bestBuyPrice) {
            break;  // No match possible
        }

        // Match with front order (FIFO within price level)
        auto& buyQueue = bestBuyLevel->second;
        auto matchedOrder = buyQueue.front();

        int matchQty = std::min(incoming->getRemainingQuantity(),
                               matchedOrder->getRemainingQuantity());

        incoming->reduceQuantity(matchQty);
        matchedOrder->reduceQuantity(matchQty);

        // Generate report for incoming order FIRST after this match (use matched order's price)
        if (incoming->isFilled()) {
            incoming->setStatus(OrderStatus::FILLED);
            reports.push_back(generateReport(incoming, ExecutionStatus::FILL, "", matchedOrder->getPrice(), matchQty));
        } else if (incoming->getRemainingQuantity() < incoming->getQuantity()) {
            // Partially filled after this match
            incoming->setStatus(OrderStatus::PARTIALLY_FILLED);
            reports.push_back(generateReport(incoming, ExecutionStatus::PFILL, "", matchedOrder->getPrice(), matchQty));
        }

        // Then generate report for matched buy order (use its own price)
        if (matchedOrder->isFilled()) {
            matchedOrder->setStatus(OrderStatus::FILLED);
            reports.push_back(generateReport(matchedOrder, ExecutionStatus::FILL, "", -1.0, matchQty));
            buyQueue.pop_front();
        } else {
            matchedOrder->setStatus(OrderStatus::PARTIALLY_FILLED);
            reports.push_back(generateReport(matchedOrder, ExecutionStatus::PFILL, "", -1.0, matchQty));
        }

        // Remove empty price level
        if (buyQueue.empty()) {
            buyLevels_.erase(bestBuyLevel);
        }
    }

    // Handle case where incoming order had NO matches at all
    if (incoming->getRemainingQuantity() == incoming->getQuantity()) {
        addSellOrder(incoming);
        reports.push_back(generateReport(incoming, ExecutionStatus::NEW));
    } else if (!incoming->isFilled()) {
        // Had some matches but not fully filled - add to order book
        addSellOrder(incoming);
    }

    return reports;
}

ExecutionReportPtr OrderBook::generateReport(const OrderPtr& order,
                                             ExecutionStatus status,
                                             const std::string& reason,
                                             double executionPrice,
                                             int fillQtyThisMatch) {
    // Use provided fill quantity for this match, or calculate cumulative
    int reportQty;
    if (fillQtyThisMatch >= 0) {
        // Specific match quantity provided
        reportQty = fillQtyThisMatch;
    } else if (status == ExecutionStatus::NEW) {
        // For NEW orders, show original quantity
        reportQty = order->getQuantity();
    } else {
        // For FILL/PFILL without specific match qty, show cumulative filled
        reportQty = order->getQuantity() - order->getRemainingQuantity();
    }
    
    // Use provided execution price, or fall back to order's price
    double reportPrice = (executionPrice >= 0) ? executionPrice : order->getPrice();
    
    auto report = std::make_shared<ExecutionReport>(
        order->getClientOrderId(),
        order->getExchangeOrderId(),
        order->getInstrument(),
        order->getSide(),
        status,
        reportQty,
        reportPrice,
        reason
    );
    return report;
}

std::vector<OrderPtr> OrderBook::getAllBuyOrders() const {
    std::vector<OrderPtr> result;
    for (const auto& [price, queue] : buyLevels_) {
        for (const auto& order : queue) {
            result.push_back(order);
        }
    }
    return result;
}

std::vector<OrderPtr> OrderBook::getAllSellOrders() const {
    std::vector<OrderPtr> result;
    for (const auto& [price, queue] : sellLevels_) {
        for (const auto& order : queue) {
            result.push_back(order);
        }
    }
    return result;
}

void OrderBook::clear() {
    buyLevels_.clear();
    sellLevels_.clear();
}

} // namespace flower_exchange
