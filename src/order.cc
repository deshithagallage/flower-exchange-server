#include "../include/order.h"
#include <set>
#include <sstream>
#include <iomanip>

Order::Order(const std::string& clientOrdId, const std::string& instr,
             OrderSide s, double p, int q)
    : clientOrderId(clientOrdId), instrument(instr), side(s), 
      price(p), quantity(q), prioritySequence(0) {
}

bool Order::isValidInstrument(const std::string& instrument) {
    static const std::set<std::string> validInstruments = {
        "Rose", "Lavender", "Lotus", "Tulip", "Orchid"
    };
    return validInstruments.count(instrument) > 0;
}

bool Order::isValidQuantity(int quantity) {
    return quantity >= 10 && quantity <= 1000 && quantity % 10 == 0;
}

bool Order::isValid(std::string& rejectionReason) const {
    if (clientOrderId.empty() || clientOrderId.length() > 7) {
        rejectionReason = "Invalid Client Order ID";
        return false;
    }

    if (!isValidInstrument(instrument)) {
        rejectionReason = "Invalid Instrument";
        return false;
    }

    if (side != OrderSide::BUY && side != OrderSide::SELL) {
        rejectionReason = "Invalid Side";
        return false;
    }

    if (price <= 0.0) {
        rejectionReason = "Invalid Price (must be > 0)";
        return false;
    }

    if (!isValidQuantity(quantity)) {
        rejectionReason = "Invalid Quantity (must be 10-1000, multiple of 10)";
        return false;
    }

    return true;
}

ExecutionReport::ExecutionReport(const Order& order, ExecutionStatus s,
                                 const std::string& txnTime, const std::string& rej_reason)
    : clientOrderId(order.clientOrderId),
      orderId(order.orderId),
      instrument(order.instrument),
      side(order.side),
      price(order.price),
      quantity(order.quantity),
      status(s),
      reason(rej_reason),
      transactionTime(txnTime),
      executedQuantity(0),
      executedPrice(0.0) {
}

std::string ExecutionReport::toString() const {
    std::ostringstream oss;
    oss << clientOrderId << ","
        << orderId << ","
        << instrument << ","
        << static_cast<int>(side) << ","
        << std::fixed << std::setprecision(2) << price << ","
        << quantity << ","
        << static_cast<int>(status) << ",";
    
    if (!reason.empty()) {
        oss << reason;
    }
    
    oss << "," << transactionTime;
    
    return oss.str();
}
