#include "../include/order_book.h"
#include <sstream>
#include <iomanip>

OrderBook::OrderBook(const std::string& instr)
    : instrument(instr), 
      buyOrders(OrderSide::BUY),
      sellOrders(OrderSide::SELL),
      nextOrderId(1000) {
}

std::string OrderBook::generateOrderId() {
    std::ostringstream oss;
    oss << "ORD" << std::setfill('0') << std::setw(7) << nextOrderId++;
    return oss.str();
}

std::vector<std::shared_ptr<ExecutionReport>> OrderBook::processOrder(
    std::shared_ptr<Order> order,
    const std::string& transactionTime) {
    
    std::vector<std::shared_ptr<ExecutionReport>> reports;
    
    // Generate order ID
    order->orderId = generateOrderId();
    
    // Try to match against existing orders
    reports = matchOrder(order, transactionTime);
    
    // If order still has remaining quantity, add it to the book
    if (order->quantity > 0) {
        if (order->side == OrderSide::BUY) {
            buyOrders.insertOrder(order);
        } else {
            sellOrders.insertOrder(order);
        }
        
        // Create "New" execution report for unmatched quantity
        auto newReport = std::make_shared<ExecutionReport>(
            *order, ExecutionStatus::NEW, transactionTime);
        reports.push_back(newReport);
    }
    
    return reports;
}

std::vector<std::shared_ptr<ExecutionReport>> OrderBook::matchOrder(
    std::shared_ptr<Order> incomingOrder,
    const std::string& transactionTime) {
    
    std::vector<std::shared_ptr<ExecutionReport>> reports;
    int totalExecutedQty = 0;
    double executedPrice = 0.0;
    
    if (incomingOrder->side == OrderSide::BUY) {
        // Try to match against sell orders (best ask)
        while (incomingOrder->quantity > 0 && !sellOrders.isEmpty()) {
            auto bestSell = sellOrders.getBestOrder();
            
            if (bestSell->price <= incomingOrder->price) {
                // We have a match
                int executedQty = 0;
                executedPrice = bestSell->price;
                
                executeTrade(incomingOrder, bestSell, executedQty, executedPrice);
                totalExecutedQty += executedQty;
                
                if (bestSell->quantity <= 0) {
                    sellOrders.removeOrder(bestSell->orderId);
                }
            } else {
                break;
            }
        }
    } else {  // SELL order
        // Try to match against buy orders (best bid)
        while (incomingOrder->quantity > 0 && !buyOrders.isEmpty()) {
            auto bestBuy = buyOrders.getBestOrder();
            
            if (bestBuy->price >= incomingOrder->price) {
                // We have a match
                int executedQty = 0;
                executedPrice = bestBuy->price;
                
                executeTrade(incomingOrder, bestBuy, executedQty, executedPrice);
                totalExecutedQty += executedQty;
                
                if (bestBuy->quantity <= 0) {
                    buyOrders.removeOrder(bestBuy->orderId);
                }
            } else {
                break;
            }
        }
    }
    
    // Generate execution reports for matched orders
    if (totalExecutedQty > 0) {
        auto fillType = (incomingOrder->quantity > 0) ? ExecutionStatus::PFILL : ExecutionStatus::FILL;
        auto report = std::make_shared<ExecutionReport>(
            *incomingOrder, fillType, transactionTime);
        report->executedQuantity = totalExecutedQty;
        report->executedPrice = executedPrice;
        reports.push_back(report);
    }
    
    return reports;
}

void OrderBook::executeTrade(std::shared_ptr<Order>& aggressiveOrder,
                            std::shared_ptr<Order>& passiveOrder,
                            int& executedQty,
                            double& executedPrice) {
    int tradeQty = std::min(aggressiveOrder->quantity, passiveOrder->quantity);
    
    aggressiveOrder->quantity -= tradeQty;
    passiveOrder->quantity -= tradeQty;
    
    executedQty = tradeQty;
    executedPrice = passiveOrder->price;  // Passive order price
}

OrderBookSide& OrderBook::getBuySide() {
    return buyOrders;
}

const OrderBookSide& OrderBook::getBuySide() const {
    return buyOrders;
}

OrderBookSide& OrderBook::getSellSide() {
    return sellOrders;
}

const OrderBookSide& OrderBook::getSellSide() const {
    return sellOrders;
}

const std::string& OrderBook::getInstrument() const {
    return instrument;
}
