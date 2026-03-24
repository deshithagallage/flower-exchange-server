#pragma once

#include "../engine/Exchange.h"
#include "../models/Order.h"
#include "../models/ExecutionReport.h"
#include <memory>
#include <vector>

namespace flower_exchange {

/**
 * OrderService: High-level order processing with history tracking.
 */
class OrderService {
private:
    std::shared_ptr<Exchange> exchange_;                 // Reference to exchange
    std::vector<OrderPtr> order_history_;                // History of all orders
    std::vector<ExecutionReportPtr> report_history_;    // History of all execution reports

public:
    // Constructor: create service with Exchange reference, e.g., OrderService(std::make_shared<Exchange>())
    explicit OrderService(std::shared_ptr<Exchange> exchange) 
        : exchange_(exchange) {
        if (!exchange_) throw std::invalid_argument("Exchange cannot be null");
    }

    // Submit a single order for processing, e.g., submitOrder(order)
    std::vector<ExecutionReportPtr> submitOrder(OrderPtr order);

    // Bulk import orders from CSV file, e.g., bulkImportFromCSV("orders.csv")
    std::vector<ExecutionReportPtr> bulkImportFromCSV(const std::string& filename);

    // Export all execution reports to CSV, e.g., exportReportsToCSV("results.csv")
    void exportReportsToCSV(const std::string& filename) const;

    // Export all submitted orders to CSV, e.g., exportOrdersToCSV("orders.csv")
    void exportOrdersToCSV(const std::string& filename) const;

    // Get all submitted orders, e.g., getOrderHistory()
    const std::vector<OrderPtr>& getOrderHistory() const { return order_history_; }

    // Get all execution reports, e.g., getReportHistory()
    const std::vector<ExecutionReportPtr>& getReportHistory() const { return report_history_; }

    // Get the number of orders submitted, e.g., getOrderCount()
    int getOrderCount() const { return order_history_.size(); }

    // Get the number of execution reports, e.g., getReportCount()
    int getReportCount() const { return report_history_.size(); }

    // Get all orders for a specific client, e.g., getOrdersByClient("C1")
    std::vector<OrderPtr> getOrdersByClient(const std::string& client_id) const;

    // Get a single order by its ID, e.g., getOrderById("O1")
    OrderPtr getOrderById(const std::string& order_id) const;

    // Clear only the service's history
    void clearHistory() { order_history_.clear(); report_history_.clear(); }

    // Clear both service history and exchange order books
    void clearAll() { exchange_->clear(); clearHistory(); }

    // Get the underlying exchange reference for advanced use, e.g., getExchange()
    std::shared_ptr<Exchange> getExchange() const { return exchange_; }
};

} 