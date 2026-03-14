#include <iostream>
#include <drogon/drogon.h>
#include "engine/Exchange.h"
#include "services/OrderService.h"
#include "controllers/OrderCtrl.h"

using namespace flower_exchange;
using namespace drogon;

int main() {
    try {
        // Initialize exchange and service
        auto exchange = std::make_shared<Exchange>();
        OrderCtrl::orderService_ = std::make_shared<OrderService>(exchange);

        // Log server startup
        LOG_INFO << "Flower Exchange Server v1.0.0";
        LOG_INFO << "Platform: C++20 with Drogon Framework";
        LOG_INFO << "Listening on 0.0.0.0:5555";

        // Start HTTP server - Drogon auto-discovers controllers
        app().setLogLevel(trantor::Logger::kInfo);
        app().addListener("0.0.0.0", 5555);
        app().run();
        
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
