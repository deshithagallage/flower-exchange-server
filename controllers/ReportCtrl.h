#pragma once
#include <drogon/HttpController.h>
#include <fstream>
#include <sstream>
using namespace drogon;

namespace flower_exchange {

class ReportCtrl : public HttpController<ReportCtrl> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ReportCtrl::downloadReports, "/api/reports/download", Get);
    METHOD_LIST_END

    void downloadReports(const HttpRequestPtr& req,
                        std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace flower_exchange
