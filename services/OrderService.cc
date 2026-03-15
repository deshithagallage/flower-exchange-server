#include "OrderService.h"

namespace flower_exchange {

OrderService::OrderService(const std::shared_ptr<Exchange>& exchange)
    : exchange_(exchange) {}

std::vector<ExecutionReportPtr> OrderService::submitOrder(const OrderPtr& order) {
    if (!exchange_) {
        return {};
    }
    auto reports = exchange_->processOrder(order);
    
    // Persist reports to CSV
    CSVWriter::writeReportsToFile("/data/execution_reports.csv", reports);
    
    return reports;
}

std::vector<OrderPtr> OrderService::getOrdersForInstrument(Instrument instrument) {
    if (!exchange_) {
        return {};
    }
    const auto& orderBook = exchange_->getOrderBook(instrument);
    auto buyOrders = orderBook.getAllBuyOrders();
    auto sellOrders = orderBook.getAllSellOrders();
    
    buyOrders.insert(buyOrders.end(), sellOrders.begin(), sellOrders.end());
    return buyOrders;
}

std::vector<OrderPtr> OrderService::getBuyOrders(Instrument instrument) const {
    if (!exchange_) {
        return {};
    }
    return exchange_->getOrderBook(instrument).getAllBuyOrders();
}

std::vector<OrderPtr> OrderService::getSellOrders(Instrument instrument) const {
    if (!exchange_) {
        return {};
    }
    return exchange_->getOrderBook(instrument).getAllSellOrders();
}

const std::shared_ptr<Exchange>& OrderService::getExchange() const {
    return exchange_;
}

void OrderService::reset() {
    if (exchange_) {
        exchange_->clear();
    }
}

int OrderService::submitBulkOrders(const std::string& csvContent) {
    if (!exchange_ || csvContent.empty()) {
        return 0;
    }
    
    // Parse orders from CSV content
    auto orders = CSVReader::readOrdersFromString(csvContent);
    
    // Process each order
    int successCount = 0;
    for (const auto& order : orders) {
        auto reports = submitOrder(order);  // submitOrder also writes to CSV
        if (!reports.empty()) {
            successCount++;
        }
    }
    
    return successCount;
}

} // namespace flower_exchange
