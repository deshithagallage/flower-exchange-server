#pragma once
#include <drogon/HttpController.h>
using namespace drogon;

namespace flower_exchange {

class HealthCtrl : public HttpController<HealthCtrl> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(HealthCtrl::healthCheck, "/api/health", Get);
    METHOD_LIST_END

    void healthCheck(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace flower_exchange
