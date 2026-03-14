#pragma once

#include "Enums.h"
#include "../utils/EnumConverters.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <memory>

namespace flower_exchange {

class ExecutionReport {
public:
    ExecutionReport(const std::string& clientOrderId,
                   const std::string& exchangeOrderId,
                   Instrument instrument,
                   Side side,
                   ExecutionStatus status,
                   int quantity,
                   double price,
                   const std::string& reason = "");

    // Getters
    const std::string& getClientOrderId() const { return clientOrderId_; }
    const std::string& getExchangeOrderId() const { return exchangeOrderId_; }
    Instrument getInstrument() const { return instrument_; }
    Side getSide() const { return side_; }
    ExecutionStatus getStatus() const { return executionStatus_; }
    int getQuantity() const { return quantity_; }
    double getPrice() const { return price_; }
    const std::string& getReason() const { return reason_; }
    const std::string& getTimestamp() const { return timestamp_; }

    std::string toCSVRow() const {
        std::ostringstream oss;
        oss << clientOrderId_ << ","
            << exchangeOrderId_ << ","
            << instrumentStr(instrument_) << ","
            << sideStr(side_) << ","
            << executionStatusStr(executionStatus_) << ","
            << quantity_ << ","
            << std::fixed << std::setprecision(2) << price_ << ","
            << reason_ << ","
            << timestamp_;
        return oss.str();
    }

private:
    std::string clientOrderId_;
    std::string exchangeOrderId_;
    Instrument instrument_;
    Side side_;
    ExecutionStatus executionStatus_;
    int quantity_;
    double price_;
    std::string reason_;
    std::string timestamp_;
};

using ExecutionReportPtr = std::shared_ptr<ExecutionReport>;

} // namespace flower_exchange
