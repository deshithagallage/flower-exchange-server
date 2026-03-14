#pragma once

#include <drogon/HttpSimpleController.h>
#include "../include/exchange.h"

using namespace drogon;

class ExchangeCtrl : public drogon::HttpSimpleController<ExchangeCtrl> {
public:
    ExchangeCtrl();
    void asyncHandleHttpRequest(const HttpRequestPtr& req, 
                                std::function<void(const HttpResponsePtr&)>&& callback) override;
    
    PATH_LIST_BEGIN
    PATH_ADD("/exchange/health", Get);
    PATH_ADD("/exchange/process", Post);
    PATH_ADD("/exchange/orders", Post);
    // PATH_ADD("/exchange/orderbook/*", Get);
    PATH_LIST_END
    
private:
    std::shared_ptr<Exchange> exchange;
};

