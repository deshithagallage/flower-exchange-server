#pragma once

#include "Enums.h"
#include <string>
#include <memory>

namespace flower_exchange {

/**
 * Represents the result of processing an order.
 * 
 * Generated when:
 * - Order is queued (no matches)
 * - Order is matched (full or partial fill)
 * - Order is rejected (validation failure)
 * 
 * Immutable after creation - represents a historical record.
 */
class ExecutionReport {
private:
    std::string exchange_order_id_;     
    std::string timestamp_;             
    ExecutionStatus status_;            
    int filled_quantity_;               
    double execution_price_;            
    std::string reason_;                
    Instrument instrument_;             
    Side side_;                        

public:
    /**
     * Constructor: Create an execution report.
     * 
     * @param exchange_order_id Order this report is for
     * @param instrument Which flower
     * @param side BUY or SELL
     * @param status QUEUED, FILLED, or REJECTED
     * @param filled_quantity Units matched (0 if queued/rejected)
     * @param execution_price Price matched at (0 if queued/rejected)
     * @param reason Rejection reason (empty if filled/queued)
     * @param timestamp When this happened
     */
    ExecutionReport(const std::string& exchange_order_id,
                   Instrument instrument,
                   Side side,
                   ExecutionStatus status,
                   int filled_quantity = 0,
                   double execution_price = 0.0,
                   const std::string& reason = "",
                   const std::string& timestamp = "");


    const std::string& getExchangeOrderId() const { return exchange_order_id_; }
    const std::string& getTimestamp() const { return timestamp_; }
    ExecutionStatus getStatus() const { return status_; }
    int getFilledQuantity() const { return filled_quantity_; }
    double getExecutionPrice() const { return execution_price_; }
    const std::string& getReason() const { return reason_; }
    Instrument getInstrument() const { return instrument_; }
    Side getSide() const { return side_; }


    bool isQueued() const { return status_ == ExecutionStatus::QUEUED; }

    bool isFilled() const { return status_ == ExecutionStatus::FILLED; }

    bool isRejected() const { return status_ == ExecutionStatus::REJECTED; }

    double getExecutionValue() const {
        return filled_quantity_ * execution_price_;
    }
};

/**
 * Smart pointer type for ExecutionReport.
 * Use this everywhere.
 */
using ExecutionReportPtr = std::shared_ptr<ExecutionReport>;

}