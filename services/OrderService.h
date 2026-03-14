#pragma once
#include "../engine/Exchange.h"
#include "../models/ExecutionReport.h"
#include "../utils/CSVWriter.h"
#include "../utils/CSVReader.h"
#include <memory>
#include <vector>

namespace flower_exchange {

class OrderService {
public:
    explicit OrderService(const std::shared_ptr<Exchange>& exchange);

    std::vector<ExecutionReportPtr> submitOrder(const OrderPtr& order);

    std::vector<OrderPtr> getOrdersForInstrument(Instrument instrument);

    std::vector<OrderPtr> getBuyOrders(Instrument instrument) const;

    std::vector<OrderPtr> getSellOrders(Instrument instrument) const;

    // Bulk operations
    int submitBulkOrders(const std::string& csvContent);

    const std::shared_ptr<Exchange>& getExchange() const;

    void reset();

private:
    std::shared_ptr<Exchange> exchange_;
};

} // namespace flower_exchange
