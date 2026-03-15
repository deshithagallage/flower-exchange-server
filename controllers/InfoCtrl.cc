#include "InfoCtrl.h"
#include <json/json.h>
#include <cstdlib>
using namespace drogon;

void InfoCtrl::asyncHandleHttpRequest(const HttpRequestPtr &req,
                                      std::function<void(const HttpResponsePtr &)> &&callback)
{
    Json::Value json;
    json["status"] = "ok";
    json["service"] = "flower-exchange-backend";
    
    // Read version from environment variable, default to 1.0.0
    const char* versionEnv = std::getenv("VERSION");
    json["version"] = versionEnv ? versionEnv : "1.0.0";
    
    auto resp = HttpResponse::newHttpResponse();
    resp->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
    resp->setBody(json.toStyledString());
    resp->setExpiredTime(0);
    callback(resp);
}
