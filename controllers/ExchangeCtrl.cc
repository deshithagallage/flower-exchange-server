#include "ExchangeCtrl.h"
#include <json/json.h>
#include <sstream>

ExchangeCtrl::ExchangeCtrl() : exchange(std::make_shared<Exchange>()) {
}

void ExchangeCtrl::asyncHandleHttpRequest(const HttpRequestPtr& req,
                                          std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        std::string path = req->getPath();
        std::string method = req->getMethodString();
        
        if (path == "/exchange/health" && method == "GET") {
            // Health check
            Json::Value json;
            json["status"] = "healthy";
            json["service"] = "Flower Exchange Engine";
            json["version"] = "1.0.0";
            
            auto resp = HttpResponse::newHttpJsonResponse(json);
            resp->setStatusCode(k200OK);
            callback(resp);
        }
        else if (path == "/exchange/process" && method == "POST") {
            // Process single order
            auto json = req->getJsonObject();
            
            if (!json || json->isNull()) {
                Json::Value errorJson;
                errorJson["error"] = "Invalid JSON";
                auto resp = HttpResponse::newHttpJsonResponse(errorJson);
                resp->setStatusCode(k400BadRequest);
                callback(resp);
                return;
            }
            
            std::string clientOrderId = (*json)["clientOrderId"].asString();
            std::string instrument = (*json)["instrument"].asString();
            int side = (*json)["side"].asInt();
            double price = (*json)["price"].asDouble();
            int quantity = (*json)["quantity"].asInt();
            
            auto reports = exchange->processOrder(clientOrderId, instrument, side, price, quantity);
            
            Json::Value responseJson;
            responseJson["success"] = true;
            responseJson["reportCount"] = (int)reports.size();
            
            Json::Value reportsArray(Json::arrayValue);
            for (const auto& report : reports) {
                Json::Value reportJson;
                reportJson["clientOrderId"] = report->clientOrderId;
                reportJson["orderId"] = report->orderId;
                reportJson["instrument"] = report->instrument;
                reportJson["side"] = static_cast<int>(report->side);
                reportJson["price"] = report->price;
                reportJson["quantity"] = report->quantity;
                reportJson["status"] = static_cast<int>(report->status);
                reportJson["reason"] = report->reason;
                reportJson["transactionTime"] = report->transactionTime;
                reportsArray.append(reportJson);
            }
            responseJson["reports"] = reportsArray;
            
            auto resp = HttpResponse::newHttpJsonResponse(responseJson);
            resp->setStatusCode(k200OK);
            callback(resp);
        }
        else if (path == "/exchange/orders" && method == "POST") {
            // Batch process orders from CSV file upload
            auto contentTypeView = req->getHeader("Content-Type");
            std::string contentType{contentTypeView};
            
            // Check if it's multipart form data or CSV raw body
            if (contentType.find("multipart/form-data") != std::string::npos) {
                // Handle multipart file upload
                auto bodyView = req->getBody();
                std::string body{bodyView};
                if (body.empty()) {
                    Json::Value errorJson;
                    errorJson["error"] = "No file uploaded";
                    auto resp = HttpResponse::newHttpJsonResponse(errorJson);
                    resp->setStatusCode(k400BadRequest);
                    callback(resp);
                    return;
                }
                
                // Extract boundary from Content-Type header
                size_t boundaryPos = contentType.find("boundary=");
                if (boundaryPos == std::string::npos) {
                    Json::Value errorJson;
                    errorJson["error"] = "Invalid multipart form data";
                    auto resp = HttpResponse::newHttpJsonResponse(errorJson);
                    resp->setStatusCode(k400BadRequest);
                    callback(resp);
                    return;
                }
                
                std::string boundary = contentType.substr(boundaryPos + 9);
                // Remove any trailing characters like quotes or semicolons
                size_t endPos = boundary.find_first_of(";\r\n");
                if (endPos != std::string::npos) {
                    boundary = boundary.substr(0, endPos);
                }
                
                // Find the file content between boundaries
                std::string delimiter = "--" + boundary;
                size_t startPos = body.find(delimiter);
                if (startPos == std::string::npos) {
                    Json::Value errorJson;
                    errorJson["error"] = "No file data found";
                    auto resp = HttpResponse::newHttpJsonResponse(errorJson);
                    resp->setStatusCode(k400BadRequest);
                    callback(resp);
                    return;
                }
                
                // Find the next boundary to know where the file ends
                size_t nextBoundary = body.find(delimiter, startPos + delimiter.length());
                
                // Extract the file section
                std::string fileSection = body.substr(startPos, nextBoundary - startPos);
                
                // Find the actual file data (after headers)
                size_t dataStart = fileSection.find("\r\n\r\n");
                if (dataStart == std::string::npos) {
                    dataStart = fileSection.find("\n\n");
                    if (dataStart == std::string::npos) {
                        Json::Value errorJson;
                        errorJson["error"] = "Invalid file format";
                        auto resp = HttpResponse::newHttpJsonResponse(errorJson);
                        resp->setStatusCode(k400BadRequest);
                        callback(resp);
                        return;
                    }
                    dataStart += 2;
                } else {
                    dataStart += 4;
                }
                
                std::string csvContent = fileSection.substr(dataStart);
                // Remove trailing CRLF before boundary
                if (csvContent.length() >= 2 && csvContent.substr(csvContent.length() - 2) == "\r\n") {
                    csvContent = csvContent.substr(0, csvContent.length() - 2);
                }
                
                // Process CSV content
                std::istringstream csvStream(csvContent);
                std::string line;
                std::vector<std::shared_ptr<ExecutionReport>> allReports;
                int processedCount = 0;
                
                // Skip header line
                std::getline(csvStream, line);
                
                while (std::getline(csvStream, line)) {
                    if (line.empty()) continue;
                    
                    std::istringstream lineStream(line);
                    std::string clientOrderId, instrument, sideStr, priceStr, quantityStr;
                    
                    std::getline(lineStream, clientOrderId, ',');
                    std::getline(lineStream, instrument, ',');
                    std::getline(lineStream, sideStr, ',');
                    std::getline(lineStream, priceStr, ',');
                    std::getline(lineStream, quantityStr, ',');
                    
                    try {
                        // Trim whitespace
                        auto trim = [](std::string& s) {
                            s.erase(0, s.find_first_not_of(" \t\r\n"));
                            s.erase(s.find_last_not_of(" \t\r\n") + 1);
                        };
                        trim(clientOrderId);
                        trim(instrument);
                        trim(sideStr);
                        trim(priceStr);
                        trim(quantityStr);
                        
                        int side = std::stoi(sideStr);
                        double price = std::stod(priceStr);
                        int quantity = std::stoi(quantityStr);
                        
                        auto reports = exchange->processOrder(clientOrderId, instrument, side, price, quantity);
                        for (const auto& report : reports) {
                            allReports.push_back(report);
                        }
                        processedCount++;
                    } catch (const std::exception&) {
                        continue;
                    }
                }
                
                Json::Value responseJson;
                responseJson["success"] = true;
                responseJson["ordersProcessed"] = processedCount;
                responseJson["reportsGenerated"] = (int)allReports.size();
                
                Json::Value reportsArray(Json::arrayValue);
                for (const auto& report : allReports) {
                    Json::Value reportJson;
                    reportJson["clientOrderId"] = report->clientOrderId;
                    reportJson["orderId"] = report->orderId;
                    reportJson["instrument"] = report->instrument;
                    reportJson["side"] = static_cast<int>(report->side);
                    reportJson["price"] = report->price;
                    reportJson["quantity"] = report->quantity;
                    reportJson["status"] = static_cast<int>(report->status);
                    reportJson["reason"] = report->reason;
                    reportJson["transactionTime"] = report->transactionTime;
                    reportsArray.append(reportJson);
                }
                responseJson["reports"] = reportsArray;
                
                auto resp = HttpResponse::newHttpJsonResponse(responseJson);
                resp->setStatusCode(k200OK);
                callback(resp);
            } else {
                // Fallback: accept JSON array format
                auto json = req->getJsonObject();
                
                if (!json || !(*json)["orders"].isArray()) {
                    Json::Value errorJson;
                    errorJson["error"] = "Invalid request - upload CSV file or send JSON with 'orders' array";
                    auto resp = HttpResponse::newHttpJsonResponse(errorJson);
                    resp->setStatusCode(k400BadRequest);
                    callback(resp);
                    return;
                }
                
                const auto& ordersArray = (*json)["orders"];
                std::vector<std::shared_ptr<ExecutionReport>> allReports;
                int processedCount = 0;
                
                for (const auto& orderItem : ordersArray) {
                    try {
                        std::string clientOrderId = orderItem["clientOrderId"].asString();
                        std::string instrument = orderItem["instrument"].asString();
                        int side = orderItem["side"].asInt();
                        double price = orderItem["price"].asDouble();
                        int quantity = orderItem["quantity"].asInt();
                        
                        auto reports = exchange->processOrder(clientOrderId, instrument, side, price, quantity);
                        for (const auto& report : reports) {
                            allReports.push_back(report);
                        }
                        processedCount++;
                    } catch (const std::exception&) {
                        continue;
                    }
                }
                
                Json::Value responseJson;
                responseJson["success"] = true;
                responseJson["ordersProcessed"] = processedCount;
                responseJson["reportsGenerated"] = (int)allReports.size();
                
                Json::Value reportsArray(Json::arrayValue);
                for (const auto& report : allReports) {
                    Json::Value reportJson;
                    reportJson["clientOrderId"] = report->clientOrderId;
                    reportJson["orderId"] = report->orderId;
                    reportJson["instrument"] = report->instrument;
                    reportJson["side"] = static_cast<int>(report->side);
                    reportJson["price"] = report->price;
                    reportJson["quantity"] = report->quantity;
                    reportJson["status"] = static_cast<int>(report->status);
                    reportJson["reason"] = report->reason;
                    reportJson["transactionTime"] = report->transactionTime;
                    reportsArray.append(reportJson);
                }
                responseJson["reports"] = reportsArray;
                
                auto resp = HttpResponse::newHttpJsonResponse(responseJson);
                resp->setStatusCode(k200OK);
                callback(resp);
            }
        }
        else {
            Json::Value errorJson;
            errorJson["error"] = "Not found";
            auto resp = HttpResponse::newHttpJsonResponse(errorJson);
            resp->setStatusCode(k404NotFound);
            callback(resp);
        }
    } catch (const std::exception& e) {
        Json::Value errorJson;
        errorJson["error"] = e.what();
        
        auto resp = HttpResponse::newHttpJsonResponse(errorJson);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
    }
}
