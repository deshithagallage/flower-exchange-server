#include <drogon/drogon.h>
#include <iostream>

int main() {
    int port = 5555;
    const char *portEnv = std::getenv("PORT");
    if (portEnv) {
        port = std::atoi(portEnv);
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Flower Exchange - Drogon Backend" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Starting server on port: " << port << std::endl;
    std::cout << "\nAPI Endpoints:" << std::endl;
    std::cout << "  GET  /exchange/health          - Health check" << std::endl;
    std::cout << "  POST /exchange/process         - Process single order (JSON)" << std::endl;
    std::cout << "  POST /exchange/orders          - Batch process orders (JSON array)" << std::endl;
    std::cout << "  GET  /exchange/orderbook/:name - Get order book status" << std::endl;
    std::cout << "\nSupported flowers: Rose, Lavender, Lotus, Tulip, Orchid" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    drogon::app().addListener("0.0.0.0", port);
    drogon::app().run();
    
    return 0;
}
