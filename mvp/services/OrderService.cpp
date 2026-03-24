#include "OrderService.h"
#include "../utils/CSVReader.h"
#include "../utils/CSVWriter.h"

namespace flower_exchange {

// Submit a single order to the exchange and track it, e.g., submitOrder(order)
std::vector<ExecutionReportPtr> OrderService::submitOrder(OrderPtr order) {
    if (!order) throw std::invalid_argument("Order cannot be null");

    order_history_.push_back(order);  // Track order in history

    std::vector<ExecutionReportPtr> reports = exchange_->processOrder(order); // Submit to exchange

    for (const auto& report : reports) {  // Track reports in history
        report_history_.push_back(report);
    }

    return reports;
}

// Bulk import orders from CSV and submit each, e.g., bulkImportFromCSV("orders.csv")
std::vector<ExecutionReportPtr> OrderService::bulkImportFromCSV(const std::string& filename) {
    CSVReader reader;
    auto orders = reader.readOrders(filename);

    std::vector<ExecutionReportPtr> all_reports;

    for (const auto& order : orders) {  // Submit each order
        auto reports = submitOrder(order);
        all_reports.insert(all_reports.end(), reports.begin(), reports.end());
    }

    return all_reports;
}

// Export all execution reports to a CSV file, e.g., exportReportsToCSV("results.csv")
void OrderService::exportReportsToCSV(const std::string& filename) const {
    CSVWriter writer;
    writer.writeExecutionReports(report_history_, filename);
}

// Export all submitted orders to a CSV file, e.g., exportOrdersToCSV("orders.csv")
void OrderService::exportOrdersToCSV(const std::string& filename) const {
    CSVWriter writer;
    writer.writeOrders(order_history_, filename);
}

// Get all orders submitted by a specific client, e.g., getOrdersByClient("C1")
std::vector<OrderPtr> OrderService::getOrdersByClient(const std::string& client_id) const {
    std::vector<OrderPtr> client_orders;

    for (const auto& order : order_history_) {
        if (order->getClientId() == client_id) {
            client_orders.push_back(order);
        }
    }

    return client_orders;
}

// Get a single order by its order ID, e.g., getOrderById("O1")
OrderPtr OrderService::getOrderById(const std::string& order_id) const {
    for (const auto& order : order_history_) {
        if (order->getClientOrderId() == order_id) {
            return order;
        }
    }
    return nullptr;
}

} 