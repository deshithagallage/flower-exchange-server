#include "ReportCtrl.h"
using namespace drogon;

namespace flower_exchange {

void ReportCtrl::downloadReports(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback) {
    std::ifstream file("execution_reports.csv");
    if (!file.is_open()) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k404NotFound);
        resp->setBody("Execution reports file not found");
        callback(resp);
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(HttpStatusCode::k200OK);
    resp->addHeader("Content-Type", "text/csv");
    resp->addHeader("Content-Disposition", "attachment; filename=execution_reports.csv");
    resp->setBody(buffer.str());
    callback(resp);
}

} // namespace flower_exchange
