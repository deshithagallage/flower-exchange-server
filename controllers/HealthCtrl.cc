#include "HealthCtrl.h"
using namespace drogon;

namespace flower_exchange {

void HealthCtrl::healthCheck(const HttpRequestPtr& req,
                            std::function<void(const HttpResponsePtr&)>&& callback) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(HttpStatusCode::k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    resp->setBody("{\"status\":\"healthy\"}");
    callback(resp);
}

} // namespace flower_exchange
