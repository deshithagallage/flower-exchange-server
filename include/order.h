#pragma once

#include <string>
#include <cstdint>

enum class OrderSide : int {
    BUY = 1,
    SELL = 2
};

enum class ExecutionStatus : int {
    NEW = 0,
    REJECTED = 1,
    FILL = 2,
    PFILL = 3  // Partial Fill
};

class Order {
public:
    std::string clientOrderId;
    std::string instrument;
    OrderSide side;
    double price;
    int quantity;
    std::string orderId;  // System generated
    int prioritySequence;  // For time priority within same price level

    Order() = default;
    Order(const std::string& clientOrdId, const std::string& instr, 
          OrderSide s, double p, int q);

    // Validation
    static bool isValidInstrument(const std::string& instrument);
    static bool isValidQuantity(int quantity);
    bool isValid(std::string& rejectionReason) const;
};

class ExecutionReport {
public:
    std::string clientOrderId;
    std::string orderId;
    std::string instrument;
    OrderSide side;
    double price;
    int quantity;
    ExecutionStatus status;
    std::string reason;  // For rejections
    std::string transactionTime;  // YYYYMMDD-HHMMSS.sss format
    int executedQuantity;
    double executedPrice;

    ExecutionReport() = default;
    ExecutionReport(const Order& order, ExecutionStatus s, 
                   const std::string& txnTime, const std::string& rej_reason = "");
    
    std::string toString() const;
};
