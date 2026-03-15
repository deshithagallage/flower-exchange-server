#include "OrderService.h"
#include <cstdlib>
#include <filesystem>

namespace flower_exchange {

static std::string getDataDir() {
    const char* dataDir = std::getenv("DATA_DIR");
    if (dataDir) {
        return std::string(dataDir);
    }
    return "./data";
}

static void ensureDataDirExists() {
    std::string dataDir = getDataDir();
    std::filesystem::create_directories(dataDir);
}

OrderService::OrderService(const std::shared_ptr<Exchange>& exchange)
    : exchange_(exchange) {}

std::vector<ExecutionReportPtr> OrderService::submitOrder(const OrderPtr& order) {
    if (!exchange_) {
        return {};
    }
    auto reports = exchange_->processOrder(order);
    
    // Persist reports to CSV
    ensureDataDirExists();
    std::string reportPath = getDataDir() + "/execution_reports.csv";
    CSVWriter::writeReportsToFile(reportPath, reports);
    
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
