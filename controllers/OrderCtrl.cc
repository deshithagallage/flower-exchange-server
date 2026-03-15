#include "OrderCtrl.h"
#include "../utils/EnumConverters.h"
#include <json/json.h>
using namespace drogon;

namespace flower_exchange {

std::shared_ptr<OrderService> OrderCtrl::orderService_;

void OrderCtrl::addOrder(const HttpRequestPtr& req,
                        std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        auto jsonObj = req->getJsonObject();
        if (!jsonObj) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(HttpStatusCode::k400BadRequest);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            resp->setBody("{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
            callback(resp);
            return;
        }

        // Extract order fields from JSON - allow empty/invalid values to be processed as rejections
        std::string clientId = jsonObj->get("clientId", "").asString();
        std::string clientOrderId = jsonObj->get("clientOrderId", "").asString();
        std::string instrumentStr = jsonObj->get("instrument", "").asString();
        std::string sideStr = jsonObj->get("side", "").asString();
        double price = jsonObj->get("price", 0.0).asDouble();
        int quantity = jsonObj->get("quantity", 0).asInt();

        // Create Order object with provided or default values
        // Invalid values will be caught by validators and return REJECTED reports
        auto order = std::make_shared<Order>(
            clientId,
            clientOrderId,
            strToInstrument(instrumentStr),
            strToSide(sideStr),
            price,
            quantity
        );

        LOG_INFO << "Processing order: " << clientOrderId;

        // Submit order through service (service handles report persistence)
        auto reports = orderService_->submitOrder(order);

        LOG_INFO << "Generated " << reports.size() << " execution reports for order " << clientOrderId;

        // Build response with all reports (including REJECTED ones)
        Json::Value responseJson;
        responseJson["status"] = "success";
        responseJson["clientOrderId"] = clientOrderId;
        responseJson["reportsGenerated"] = static_cast<int>(reports.size());
        responseJson["reports"] = Json::arrayValue;

        for (const auto& report : reports) {
            Json::Value reportJson;
            reportJson["exchangeOrderId"] = report->getExchangeOrderId();
            reportJson["status"] = executionStatusStr(report->getStatus());
            reportJson["quantity"] = report->getQuantity();
            reportJson["price"] = report->getPrice();
            reportJson["reason"] = report->getReason();
            responseJson["reports"].append(reportJson);
        }

        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody(responseJson.toStyledString());
        callback(resp);

    } catch (const std::exception& e) {
        LOG_ERROR << "Error processing order: " << e.what();
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody("{\"status\":\"error\",\"message\":\"" + std::string(e.what()) + "\"}");
        callback(resp);
    }
}

void OrderCtrl::uploadOrders(const HttpRequestPtr& req,
                            std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        // Get the raw body which contains multipart file data
        auto bodyView = req->getBody();
        std::string body(bodyView.begin(), bodyView.end());
        
        LOG_INFO << "Received bulk upload body size: " << body.size() << " bytes";
        
        if (body.empty()) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(HttpStatusCode::k400BadRequest);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            resp->setBody("{\"status\":\"error\",\"message\":\"No data received\"}");
            callback(resp);
            return;
        }

        // Extract CSV content from multipart body
        size_t startPos = body.find("\r\n\r\n");
        if (startPos == std::string::npos) {
            startPos = body.find("\n\n");
            if (startPos == std::string::npos) {
                LOG_WARN << "Could not find content separator in multipart body";
                auto resp = HttpResponse::newHttpResponse();
                resp->setStatusCode(HttpStatusCode::k400BadRequest);
                resp->setContentTypeCode(CT_APPLICATION_JSON);
                resp->setBody("{\"status\":\"error\",\"message\":\"Invalid file format\"}");
                callback(resp);
                return;
            }
            startPos += 2;
        } else {
            startPos += 4;
        }
        
        // Find the end boundary
        size_t endPos = body.rfind("\r\n--");
        if (endPos == std::string::npos) {
            endPos = body.rfind("\n--");
        }
        if (endPos == std::string::npos) {
            endPos = body.length();
        }
        
        // Extract CSV content
        std::string csvContent = body.substr(startPos, endPos - startPos);
        
        // Trim trailing whitespace
        while (!csvContent.empty() && (csvContent.back() == '\r' || csvContent.back() == '\n')) {
            csvContent.pop_back();
        }
        
        LOG_INFO << "Extracted CSV content size: " << csvContent.size() << " bytes";
        
        if (csvContent.empty()) {
            LOG_WARN << "CSV content is empty after extraction";
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(HttpStatusCode::k400BadRequest);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            resp->setBody("{\"status\":\"error\",\"message\":\"File is empty\"}");
            callback(resp);
            return;
        }

        LOG_INFO << "CSV content to parse: " << csvContent;
        
        // Process orders through service (handles parsing and persistence)
        auto reports = orderService_->submitBulkOrders(csvContent);

        LOG_INFO << "Generated " << reports.size() << " execution reports from bulk upload";
        
        Json::Value responseJson;
        responseJson["status"] = "success";
        responseJson["reportsGenerated"] = static_cast<int>(reports.size());
        responseJson["reports"] = Json::arrayValue;

        for (const auto& report : reports) {
            Json::Value reportJson;
            reportJson["clientOrderId"] = report->getClientOrderId();
            reportJson["exchangeOrderId"] = report->getExchangeOrderId();
            reportJson["status"] = executionStatusStr(report->getStatus());
            reportJson["quantity"] = report->getQuantity();
            reportJson["price"] = report->getPrice();
            reportJson["reason"] = report->getReason();
            responseJson["reports"].append(reportJson);
        }
        
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody(responseJson.toStyledString());
        callback(resp);
    } catch (const std::exception& e) {
        LOG_ERROR << "Error in bulk upload: " << e.what();
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody("{\"status\":\"error\",\"message\":\"" + std::string(e.what()) + "\"}");
        callback(resp);
    }
}

void OrderCtrl::getAllOrders(const HttpRequestPtr& req,
                            std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        Json::Value responseJson;
        responseJson["status"] = "success";
        responseJson["orders"] = Json::arrayValue;
        
        // Get orders from all instruments (0-4: ROSE, TULIP, LILIES, SUNFLOWER, DAISY)
        for (int inst = 0; inst < 5; ++inst) {
            Instrument instrument = static_cast<Instrument>(inst);
            auto orders = orderService_->getOrdersForInstrument(instrument);
            
            for (const auto& order : orders) {
                Json::Value orderJson;
                orderJson["clientOrderId"] = order->getClientOrderId();
                orderJson["instrument"] = instrumentStr(order->getInstrument());
                orderJson["side"] = order->isBuy() ? "BUY" : "SELL";
                orderJson["price"] = order->getPrice();
                orderJson["quantity"] = order->getQuantity();
                orderJson["filledQuantity"] = order->getQuantity() - order->getRemainingQuantity();
                orderJson["remainingQuantity"] = order->getRemainingQuantity();
                
                responseJson["orders"].append(orderJson);
            }
        }

        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody(responseJson.toStyledString());
        callback(resp);

    } catch (const std::exception& e) {
        LOG_ERROR << "Error getting all orders: " << e.what();
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody("{\"status\":\"error\",\"message\":\"" + std::string(e.what()) + "\"}");
        callback(resp);
    }
}

void OrderCtrl::getOrdersByInstrument(const HttpRequestPtr& req,
                                     std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        std::string instrumentStr = req->getParameter("instrument");
        if (instrumentStr.empty()) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(HttpStatusCode::k400BadRequest);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            resp->setBody("{\"status\":\"error\",\"message\":\"instrument parameter required\"}");
            callback(resp);
            return;
        }

        Instrument instrument = strToInstrument(instrumentStr);
        auto orders = orderService_->getOrdersForInstrument(instrument);

        Json::Value responseJson;
        responseJson["status"] = "success";
        responseJson["instrument"] = instrumentStr;
        responseJson["orders"] = Json::arrayValue;

        for (const auto& order : orders) {
            Json::Value orderJson;
            orderJson["clientOrderId"] = order->getClientOrderId();
            orderJson["side"] = order->isBuy() ? "BUY" : "SELL";
            orderJson["price"] = order->getPrice();
            orderJson["quantity"] = order->getQuantity();
            orderJson["filledQuantity"] = order->getQuantity() - order->getRemainingQuantity();
            orderJson["remainingQuantity"] = order->getRemainingQuantity();
            
            responseJson["orders"].append(orderJson);
        }

        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody(responseJson.toStyledString());
        callback(resp);

    } catch (const std::exception& e) {
        LOG_ERROR << "Error getting orders by instrument: " << e.what();
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody("{\"status\":\"error\",\"message\":\"" + std::string(e.what()) + "\"}");
        callback(resp);
    }
}

void OrderCtrl::getBuyOrders(const HttpRequestPtr& req,
                            std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        std::string instrumentParam = req->getParameter("instrument");
        std::vector<OrderPtr> orders;

        if (instrumentParam.empty()) {
            // Get all buy orders across all instruments
            orders = orderService_->getAllBuyOrders();
        } else {
            // Get buy orders for specific instrument
            Instrument instrument = strToInstrument(instrumentParam);
            orders = orderService_->getBuyOrders(instrument);
        }

        Json::Value responseJson;
        responseJson["status"] = "success";
        if (!instrumentParam.empty()) {
            responseJson["instrument"] = instrumentParam;
        } else {
            responseJson["instrument"] = "ALL";
        }
        responseJson["side"] = "BUY";
        responseJson["orders"] = Json::arrayValue;

        for (const auto& order : orders) {
            Json::Value orderJson;
            orderJson["clientOrderId"] = order->getClientOrderId();
            orderJson["instrument"] = instrumentStr(order->getInstrument());
            orderJson["price"] = order->getPrice();
            orderJson["quantity"] = order->getQuantity();
            orderJson["filledQuantity"] = order->getQuantity() - order->getRemainingQuantity();
            orderJson["remainingQuantity"] = order->getRemainingQuantity();
            
            responseJson["orders"].append(orderJson);
        }

        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody(responseJson.toStyledString());
        callback(resp);

    } catch (const std::exception& e) {
        LOG_ERROR << "Error getting buy orders: " << e.what();
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody("{\"status\":\"error\",\"message\":\"" + std::string(e.what()) + "\"}");
        callback(resp);
    }
}

void OrderCtrl::getSellOrders(const HttpRequestPtr& req,
                             std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        std::string instrumentParam = req->getParameter("instrument");
        std::vector<OrderPtr> orders;

        if (instrumentParam.empty()) {
            // Get all sell orders across all instruments
            orders = orderService_->getAllSellOrders();
        } else {
            // Get sell orders for specific instrument
            Instrument instrument = strToInstrument(instrumentParam);
            orders = orderService_->getSellOrders(instrument);
        }

        Json::Value responseJson;
        responseJson["status"] = "success";
        if (!instrumentParam.empty()) {
            responseJson["instrument"] = instrumentParam;
        } else {
            responseJson["instrument"] = "ALL";
        }
        responseJson["side"] = "SELL";
        responseJson["orders"] = Json::arrayValue;

        for (const auto& order : orders) {
            Json::Value orderJson;
            orderJson["clientOrderId"] = order->getClientOrderId();
            orderJson["instrument"] = instrumentStr(order->getInstrument());
            orderJson["price"] = order->getPrice();
            orderJson["quantity"] = order->getQuantity();
            orderJson["filledQuantity"] = order->getQuantity() - order->getRemainingQuantity();
            orderJson["remainingQuantity"] = order->getRemainingQuantity();
            
            responseJson["orders"].append(orderJson);
        }

        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody(responseJson.toStyledString());
        callback(resp);

    } catch (const std::exception& e) {
        LOG_ERROR << "Error getting sell orders: " << e.what();
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody("{\"status\":\"error\",\"message\":\"" + std::string(e.what()) + "\"}");
        callback(resp);
    }
}

void OrderCtrl::resetExchange(const HttpRequestPtr& req,
                             std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        LOG_INFO << "Resetting exchange state - clearing all order books and execution reports";
        
        // Reset the exchange state
        orderService_->reset();
        
        Json::Value responseJson;
        responseJson["status"] = "success";
        responseJson["message"] = "Exchange reset successfully - all order books cleared and execution reports deleted";

        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody(responseJson.toStyledString());
        callback(resp);

    } catch (const std::exception& e) {
        LOG_ERROR << "Error resetting exchange: " << e.what();
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody("{\"status\":\"error\",\"message\":\"" + std::string(e.what()) + "\"}");
        callback(resp);
    }
}

} // namespace flower_exchange
