#include <iostream>
#include <vector>
#include <memory>
#include "include/exchange.h"
#include "include/csv_handler.h"

int main(int argc, char* argv[]) {
    try {
        // Input and output file paths
        std::string inputFile = "orders.csv";
        std::string outputFile = "execution_rep.csv";
        
        // Allow command line arguments
        if (argc > 1) {
            inputFile = argv[1];
        }
        if (argc > 2) {
            outputFile = argv[2];
        }
        
        std::cout << "Flower Exchange System - Console Mode" << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "Input file: " << inputFile << std::endl;
        std::cout << "Output file: " << outputFile << std::endl << std::endl;
        
        // Create exchange engine
        Exchange exchange;
        
        // Read orders from CSV
        std::cout << "Reading orders from " << inputFile << "..." << std::endl;
        auto orders = CSVHandler::readOrdersFromCSV(inputFile);
        std::cout << "Read " << orders.size() << " orders" << std::endl << std::endl;
        
        // Process all orders
        std::vector<std::shared_ptr<ExecutionReport>> allReports;
        int orderCount = 0;
        
        for (const auto& order : orders) {
            orderCount++;
            auto clientOrderId = std::get<0>(order);
            auto instrument = std::get<1>(order);
            auto side = std::get<2>(order);
            auto price = std::get<3>(order);
            auto quantity = std::get<4>(order);
            
            // Process order through exchange
            auto reports = exchange.processOrder(clientOrderId, instrument, side, price, quantity);
            
            // Collect all reports
            for (const auto& report : reports) {
                allReports.push_back(report);
            }
            
            if (orderCount % 100 == 0) {
                std::cout << "Processed " << orderCount << " orders..." << std::endl;
            }
        }
        
        std::cout << "Processing complete. Generated " << allReports.size() 
                  << " execution reports." << std::endl << std::endl;
        
        // Write execution reports to CSV
        std::cout << "Writing execution reports to " << outputFile << "..." << std::endl;
        if (CSVHandler::writeExecutionReportsToCSV(outputFile, allReports)) {
            std::cout << "Successfully wrote execution reports to " << outputFile << std::endl;
        } else {
            std::cerr << "Error writing to output file" << std::endl;
            return 1;
        }
        
        // Print summary statistics
        std::cout << "\n=== Order Book Summary ===" << std::endl;
        const auto& orderBooks = exchange.getAllOrderBooks();
        for (const auto& ob : orderBooks) {
            const auto& buySide = ob.second->getBuySide();
            const auto& sellSide = ob.second->getSellSide();
            std::cout << ob.first << ": Buy orders=" << buySide.getOrderCount() 
                      << ", Sell orders=" << sellSide.getOrderCount() << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
