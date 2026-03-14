#pragma once

#include "Enums.h"
#include <string>
#include <memory>

namespace flower_exchange {

class Order {
public:
    Order(const std::string& clientId,
          const std::string& clientOrderId,
          Instrument instrument,
          Side side,
          double price,
          int quantity);

    // Getters
    const std::string& getClientId() const { return clientId_; }
    const std::string& getClientOrderId() const { return clientOrderId_; }
    const std::string& getExchangeOrderId() const { return exchangeOrderId_; }
    Instrument getInstrument() const { return instrument_; }
    Side getSide() const { return side_; }
    double getPrice() const { return price_; }
    int getQuantity() const { return quantity_; }
    int getRemainingQuantity() const { return remainingQuantity_; }
    int getPrioritySequence() const { return prioritySequence_; }
    OrderStatus getStatus() const { return status_; }
    const std::string& getTimestamp() const { return timestamp_; }

    // Setters
    void setExchangeOrderId(const std::string& id) { exchangeOrderId_ = id; }
    void setPrioritySequence(int seq) { prioritySequence_ = seq; }
    void setStatus(OrderStatus status) { status_ = status; }
    void setTimestamp(const std::string& ts) { timestamp_ = ts; }

    // Business logic
    bool isBuy() const { return side_ == Side::BUY; }
    bool isSell() const { return side_ == Side::SELL; }
    void reduceQuantity(int qty) { remainingQuantity_ -= qty; }
    bool isFilled() const { return remainingQuantity_ <= 0; }

private:
    std::string clientId_;
    std::string clientOrderId_;
    std::string exchangeOrderId_;
    Instrument instrument_;
    Side side_;
    double price_;
    int quantity_;
    int remainingQuantity_;
    int prioritySequence_;
    OrderStatus status_;
    std::string timestamp_;
};

using OrderPtr = std::shared_ptr<Order>;

} // namespace flower_exchange
