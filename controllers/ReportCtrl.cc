#include "ReportCtrl.h"
#include <json/json.h>
using namespace drogon;

namespace flower_exchange {

void ReportCtrl::retrieveReports(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        const char* dataDir = std::getenv("DATA_DIR");
        std::string reportPath = (dataDir ? std::string(dataDir) : "./data") + "/execution_reports.csv";
        
        std::ifstream file(reportPath);
        if (!file.is_open()) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(HttpStatusCode::k200OK);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            resp->setBody("{\"status\":\"success\",\"reports\":[],\"total\":0}");
            callback(resp);
            return;
        }

        Json::Value reportsJson = Json::arrayValue;
        std::string line;
        bool isHeader = true;

        while (std::getline(file, line)) {
            if (isHeader) {
                isHeader = false;
                continue;
            }

            if (line.empty()) continue;

            // Parse CSV line
            std::vector<std::string> fields;
            std::stringstream ss(line);
            std::string field;

            while (std::getline(ss, field, ',')) {
                // Trim whitespace
                field.erase(0, field.find_first_not_of(" \t\r\n"));
                field.erase(field.find_last_not_of(" \t\r\n") + 1);
                fields.push_back(field);
            }

            // Expected format: ClientOrderId,ExchangeOrderId,Instrument,Side,Status,Quantity,Price,Reason,Timestamp
            if (fields.size() < 8) continue;

            try {
                Json::Value reportJson;
                reportJson["clientOrderId"] = fields[0];
                reportJson["exchangeOrderId"] = fields[1];
                reportJson["instrument"] = fields[2];
                reportJson["side"] = fields[3];
                reportJson["status"] = fields[4];
                reportJson["quantity"] = Json::Value(std::stoi(fields[5]));
                reportJson["price"] = Json::Value(std::stod(fields[6]));
                reportJson["reason"] = fields[7];
                if (fields.size() > 8) {
                    reportJson["timestamp"] = fields[8];
                }

                reportsJson.append(reportJson);
            } catch (const std::exception& e) {
                LOG_ERROR << "Error parsing report line: " << e.what();
                continue;
            }
        }

        file.close();

        Json::Value responseJson;
        responseJson["status"] = "success";
        responseJson["total"] = static_cast<int>(reportsJson.size());
        responseJson["reports"] = reportsJson;

        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody(responseJson.toStyledString());
        callback(resp);

    } catch (const std::exception& e) {
        LOG_ERROR << "Error retrieving reports: " << e.what();
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody("{\"status\":\"error\",\"message\":\"" + std::string(e.what()) + "\"}");
        callback(resp);
    }
}

void ReportCtrl::downloadReports(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback) {
    const char* dataDir = std::getenv("DATA_DIR");
    std::string reportPath = (dataDir ? std::string(dataDir) : "./data") + "/execution_reports.csv";
    std::ifstream file(reportPath);
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
