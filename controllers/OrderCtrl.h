#pragma once
#include <drogon/HttpController.h>
#include "../services/OrderService.h"
#include "../utils/CSVReader.h"
#include <memory>
using namespace drogon;

namespace flower_exchange {

class OrderCtrl : public HttpController<OrderCtrl> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(OrderCtrl::addOrder, "/api/orders/add", Post);
    ADD_METHOD_TO(OrderCtrl::uploadOrders, "/api/orders/upload", Post);
    ADD_METHOD_TO(OrderCtrl::getAllOrders, "/api/orders", Get);
    ADD_METHOD_TO(OrderCtrl::getOrdersByInstrument, "/api/orders/instrument", Get);
    ADD_METHOD_TO(OrderCtrl::getBuyOrders, "/api/orders/buy", Get);
    ADD_METHOD_TO(OrderCtrl::getSellOrders, "/api/orders/sell", Get);
    METHOD_LIST_END

    static std::shared_ptr<OrderService> orderService_;

    void addOrder(const HttpRequestPtr& req,
                 std::function<void(const HttpResponsePtr&)>&& callback);

    void uploadOrders(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback);

    void getAllOrders(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback);

    void getOrdersByInstrument(const HttpRequestPtr& req,
                              std::function<void(const HttpResponsePtr&)>&& callback);

    void getBuyOrders(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback);

    void getSellOrders(const HttpRequestPtr& req,
                      std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace flower_exchange
