#include "../include/exchange.h"
#include <chrono>
#include <iomanip>
#include <sstream>

Exchange::Exchange() {
    initializeOrderBooks();
}

void Exchange::initializeOrderBooks() {
    orderBooks["Rose"] = std::make_shared<OrderBook>("Rose");
    orderBooks["Lavender"] = std::make_shared<OrderBook>("Lavender");
    orderBooks["Lotus"] = std::make_shared<OrderBook>("Lotus");
    orderBooks["Tulip"] = std::make_shared<OrderBook>("Tulip");
    orderBooks["Orchid"] = std::make_shared<OrderBook>("Orchid");
}

std::string Exchange::getCurrentTransactionTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm* tm_info = std::localtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(tm_info, "%Y%m%d-%H%M%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

std::vector<std::shared_ptr<ExecutionReport>> Exchange::processOrder(
    const std::string& clientOrderId,
    const std::string& instrument,
    int side,
    double price,
    int quantity) {
    
    std::vector<std::shared_ptr<ExecutionReport>> reports;
    std::string transactionTime = getCurrentTransactionTime();
    
    // Create order object
    auto order = std::make_shared<Order>();
    order->clientOrderId = clientOrderId;
    order->instrument = instrument;
    order->price = price;
    order->quantity = quantity;
    
    // Validate side
    if (side == 1) {
        order->side = OrderSide::BUY;
    } else if (side == 2) {
        order->side = OrderSide::SELL;
    } else {
        order->side = static_cast<OrderSide>(side);
    }
    
    // Validate order
    std::string rejectionReason;
    if (!order->isValid(rejectionReason)) {
        auto rejectionReport = std::make_shared<ExecutionReport>(
            *order, ExecutionStatus::REJECTED, transactionTime, rejectionReason);
        rejectionReport->orderId = "REJECTED";  // Special ID for rejections
        reports.push_back(rejectionReport);
        return reports;
    }
    
    // Get order book for this instrument
    auto it = orderBooks.find(instrument);
    if (it == orderBooks.end()) {
        auto rejectionReport = std::make_shared<ExecutionReport>(
            *order, ExecutionStatus::REJECTED, transactionTime, "Invalid Instrument");
        rejectionReport->orderId = "REJECTED";
        reports.push_back(rejectionReport);
        return reports;
    }
    
    // Process order through the order book
    auto orderBook = it->second;
    reports = orderBook->processOrder(order, transactionTime);
    
    return reports;
}

std::shared_ptr<OrderBook> Exchange::getOrderBook(const std::string& instrument) const {
    auto it = orderBooks.find(instrument);
    if (it != orderBooks.end()) {
        return it->second;
    }
    return nullptr;
}

const std::map<std::string, std::shared_ptr<OrderBook>>& Exchange::getAllOrderBooks() const {
    return orderBooks;
}
