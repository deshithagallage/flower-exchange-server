#include "Exchange.h"

namespace flower_exchange {

std::string Exchange::generateExchangeOrderId() {
    return "EX_" + std::to_string(nextExchangeOrderId_++);
}

void Exchange::assignDefaultClient(OrderPtr& order) {
    if (order->getClientId().empty()) {
        // For MVP: no clientId in request, use default
        order->setTimestamp("");  // Will be regenerated
        // We can't set clientId directly since it's immutable after construction
        // For now, this is handled at CSVReader level
    }
}

std::vector<ExecutionReportPtr> Exchange::processOrder(OrderPtr order) {
    std::vector<ExecutionReportPtr> reports;

    // Assign default client if needed
    if (order->getClientId().empty()) {
        // Create a new order with default client ID
        auto newOrder = std::make_shared<Order>(
            defaultClientId_,
            order->getClientOrderId(),
            order->getInstrument(),
            order->getSide(),
            order->getPrice(),
            order->getQuantity()
        );
        order = newOrder;
    }

    // Validate order
    std::string rejectReason;
    if (!validator_.validate(*order, rejectReason)) {
        order->setStatus(OrderStatus::REJECTED);
        auto report = std::make_shared<ExecutionReport>(
            order->getClientOrderId(),
            "",  // No exchange ID yet
            order->getInstrument(),
            order->getSide(),
            ExecutionStatus::REJECTED,
            0,
            order->getPrice(),
            rejectReason
        );
        reports.push_back(report);
        return reports;
    }

    // Assign exchange order ID
    std::string exId = generateExchangeOrderId();
    order->setExchangeOrderId(exId);

    // Route to correct order book
    auto& book = getOrderBook(order->getInstrument());
    return book.processOrder(order);
}

const OrderBook& Exchange::getOrderBook(Instrument inst) const {
    return orderBooks_.at(inst);
}

OrderBook& Exchange::getOrderBook(Instrument inst) {
    return orderBooks_.at(inst);
}

void Exchange::clear() {
    for (auto& [inst, book] : orderBooks_) {
        book.clear();
    }
}

} // namespace flower_exchange
