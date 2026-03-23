#include "Exchange.h"
#include <sstream>
#include <stdexcept>

namespace flower_exchange {

// ==================== CONSTRUCTOR ====================

Exchange::Exchange() {
    order_books_.emplace(Instrument::ROSE, OrderBook(Instrument::ROSE));
    order_books_.emplace(Instrument::LAVENDER, OrderBook(Instrument::LAVENDER));
    order_books_.emplace(Instrument::LOTUS, OrderBook(Instrument::LOTUS));
    order_books_.emplace(Instrument::TULIP, OrderBook(Instrument::TULIP));
    order_books_.emplace(Instrument::ORCHID, OrderBook(Instrument::ORCHID));
}

// ==================== MAIN ENTRY POINT ====================

std::vector<ExecutionReportPtr> Exchange::processOrder(OrderPtr order) {
    std::vector<ExecutionReportPtr> reports;

    if (!order) {
        throw std::invalid_argument("Order cannot be null");
    }

    // Generate exchange order ID first (even for rejected orders)
    std::string exchange_order_id = generateExchangeOrderId();
    order->setExchangeOrderId(exchange_order_id);

    if (!validator_.isValid(order)) {
        auto report = std::make_shared<ExecutionReport>(
            exchange_order_id,
            order->getClientOrderId(),
            order->getInstrument(),
            order->getSide(),
            ExecutionStatus::REJECTED,
            order->getQuantity(),
            order->getPrice(),
            0,
            0.0,
            validator_.getLastError()
        );
        reports.push_back(report);
        order->setStatus(OrderStatus::REJECTED);
        return reports;
    }

    assignDefaultClient(order);

    order->setTimestamp("2024-03-17 12:00:00");

    try {
        OrderBook& book = order_books_.at(order->getInstrument());
        reports = book.processOrder(order);
    } catch (const std::out_of_range& e) {
        throw std::runtime_error(
            std::string("Order book not found for instrument: ") + e.what()
        );
    }

    return reports;
}

// ==================== INTERNAL HELPERS ====================

std::string Exchange::generateExchangeOrderId() {
    int next_id = next_exchange_order_id_.fetch_add(1);
    
    std::ostringstream oss;
    oss << "ord" << next_id;  // Changed from "EXO_" to "ord"
    return oss.str();
}

void Exchange::assignDefaultClient(OrderPtr& order) {
    if (order->getClientId().empty()) {
        // no-op
    }
}

// ==================== ORDER BOOK ACCESS ====================

const OrderBook& Exchange::getOrderBook(Instrument inst) const {
    return order_books_.at(inst);
}

OrderBook& Exchange::getOrderBook(Instrument inst) {
    return order_books_.at(inst);
}

// ==================== STATE MANAGEMENT ====================

bool Exchange::isEmpty() const {
    for (const auto& pair : order_books_) {
        const auto& book = pair.second;
        if (!book.isEmpty()) {
            return false;
        }
    }
    return true;
}

void Exchange::clear() {
    for (auto& pair : order_books_) {
        auto& book = pair.second;
        book.clear();
    }
}

int Exchange::getTotalOrderCount() const {
    int total = 0;
    for (const auto& pair : order_books_) {
        const auto& book = pair.second;
        total += book.getAllBuyOrders().size();
        total += book.getAllSellOrders().size();
    }
    return total;
}

// ==================== QUERY METHODS ====================

std::vector<OrderPtr> Exchange::getBuyOrders(Instrument inst) const {
    try {
        return order_books_.at(inst).getAllBuyOrders();
    } catch (const std::out_of_range&) {
        return {};
    }
}

std::vector<OrderPtr> Exchange::getSellOrders(Instrument inst) const {
    try {
        return order_books_.at(inst).getAllSellOrders();
    } catch (const std::out_of_range&) {
        return {};
    }
}

std::vector<OrderPtr> Exchange::getAllBuyOrders() const {
    std::vector<OrderPtr> all_buys;
    for (const auto& pair : order_books_) {
        const auto& book = pair.second;
        auto inst_buys = book.getAllBuyOrders();
        all_buys.insert(all_buys.end(), inst_buys.begin(), inst_buys.end());
    }
    return all_buys;
}

std::vector<OrderPtr> Exchange::getAllSellOrders() const {
    std::vector<OrderPtr> all_sells;
    for (const auto& pair : order_books_) {
        const auto& book = pair.second;
        auto inst_sells = book.getAllSellOrders();
        all_sells.insert(all_sells.end(), inst_sells.begin(), inst_sells.end());
    }
    return all_sells;
}

double Exchange::getBestBid(Instrument inst) const {
    try {
        return order_books_.at(inst).getBestBid();
    } catch (const std::out_of_range&) {
        return 0.0;
    }
}

double Exchange::getBestAsk(Instrument inst) const {
    try {
        return order_books_.at(inst).getBestAsk();
    } catch (const std::out_of_range&) {
        return 0.0;
    }
}

} 