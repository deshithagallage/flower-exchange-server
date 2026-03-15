#include "OrderService.h"
#include <cstdlib>
#include <cstdio>
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

std::vector<OrderPtr> OrderService::getAllBuyOrders() const {
    if (!exchange_) {
        return {};
    }
    std::vector<OrderPtr> allBuyOrders;
    for (Instrument inst : {Instrument::ROSE, Instrument::TULIP, Instrument::LILIES, 
                             Instrument::SUNFLOWER, Instrument::DAISY}) {
        auto buyOrders = exchange_->getOrderBook(inst).getAllBuyOrders();
        allBuyOrders.insert(allBuyOrders.end(), buyOrders.begin(), buyOrders.end());
    }
    return allBuyOrders;
}

std::vector<OrderPtr> OrderService::getAllSellOrders() const {
    if (!exchange_) {
        return {};
    }
    std::vector<OrderPtr> allSellOrders;
    for (Instrument inst : {Instrument::ROSE, Instrument::TULIP, Instrument::LILIES, 
                             Instrument::SUNFLOWER, Instrument::DAISY}) {
        auto sellOrders = exchange_->getOrderBook(inst).getAllSellOrders();
        allSellOrders.insert(allSellOrders.end(), sellOrders.begin(), sellOrders.end());
    }
    return allSellOrders;
}

const std::shared_ptr<Exchange>& OrderService::getExchange() const {
    return exchange_;
}

void OrderService::reset() {
    if (exchange_) {
        exchange_->clear();
    }
    
    // Clear CSV file
    std::string reportPath = getDataDir() + "/execution_reports.csv";
    std::remove(reportPath.c_str());
}

std::vector<ExecutionReportPtr> OrderService::submitBulkOrders(const std::string& csvContent) {
    std::vector<ExecutionReportPtr> allReports;
    
    if (!exchange_ || csvContent.empty()) {
        return allReports;
    }
    
    // Parse orders from CSV content
    auto orders = CSVReader::readOrdersFromString(csvContent);
    
    // Process each order and collect all reports
    for (const auto& order : orders) {
        auto reports = submitOrder(order);  // submitOrder also writes to CSV
        allReports.insert(allReports.end(), reports.begin(), reports.end());
    }
    
    return allReports;
}

} // namespace flower_exchange
