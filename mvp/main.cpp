#include "engine/Exchange.h"
#include "services/OrderService.h"
#include "utils/CSVReader.h"
#include "utils/CSVWriter.h"
#include <iostream>
#include <memory>
#include <iomanip>
#include <chrono>

using namespace flower_exchange;

int main(int argc, char* argv[]) {
    // Validate arguments
    if (argc != 3) {
        std::cerr << "Usage: flower-exchange <input.csv> <output.csv>\n";
        std::cerr << "\n";
        std::cerr << "  Input:  CSV file (typically 'orders.csv') with columns:\n";
        std::cerr << "          Client Order ID, Instrument, Side, Quantity, Price\n";
        std::cerr << "          Side: 1=BUY, 2=SELL\n";
        std::cerr << "          Instruments: Rose, Tulip, Lilies, Sunflower, Daisy\n";
        std::cerr << "\n";
        std::cerr << "  Output: CSV file (typically 'execution_rep.csv') with execution reports:\n";
        std::cerr << "          Order ID, Client Order ID, Instrument, Side, Exec Status, Quantity, Price\n";
        std::cerr << "          Side: 1=BUY, 2=SELL\n";
        std::cerr << "          Status: New, Fill, Rejected\n";
        std::cerr << "\n";
        std::cerr << "  Example:\n";
        std::cerr << "          flower-exchange orders.csv execution_rep.csv\n";
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];

    try {
        auto start_time = std::chrono::high_resolution_clock::now();

        // Step 1: Create exchange and service
        std::cout << "Creating exchange...\n";
        auto exchange = std::make_shared<Exchange>();
        auto service = std::make_shared<OrderService>(exchange);
        std::cout << "  ✓ Exchange with 5 instruments ready\n\n";

        // Step 2: Read orders from CSV
        std::cout << "Reading orders from: " << input_file << "\n";
        CSVReader reader;
        auto orders = reader.readOrders(input_file);
        std::cout << "  ✓ Loaded " << orders.size() << " orders\n\n";

        // Step 3: Process each order
        std::cout << "Processing orders...\n";
        std::vector<ExecutionReportPtr> all_reports;
        
        int order_count = 0;
        for (const auto& order : orders) {
            try {
                auto reports = service->submitOrder(order);
                all_reports.insert(all_reports.end(), reports.begin(), reports.end());
                order_count++;
                
                // Print progress every 100 orders
                if (order_count % 100 == 0) {
                    std::cout << "  → Processed " << order_count << " orders, "
                              << all_reports.size() << " reports generated\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "  ✗ Error processing order: " << order->getClientOrderId()
                          << " - " << e.what() << "\n";
                // Continue with next order
            }
        }
        std::cout << "  ✓ Processed " << order_count << " orders\n\n";

        // Step 4: Write results to CSV
        std::cout << "Writing reports to: " << output_file << "\n";
        CSVWriter writer;
        writer.writeExecutionReports(all_reports, output_file);
        std::cout << "  ✓ Wrote " << all_reports.size() << " execution reports\n\n";

        // Step 5: Print summary
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "═══════════════════════════════════════\n";
        std::cout << "            PROCESSING SUMMARY         \n";
        std::cout << "═══════════════════════════════════════\n";
        std::cout << "Orders processed:      " << std::setw(8) << order_count << "\n";
        std::cout << "Execution reports:     " << std::setw(8) << all_reports.size() << "\n";
        std::cout << "Processing time:       " << std::setw(8) << duration.count() << " ms\n";
        std::cout << "═══════════════════════════════════════\n\n";

        // Count reports by status
        int queued = 0, filled = 0, partial_filled = 0, rejected = 0;
        for (const auto& report : all_reports) {
            if (report->getStatus() == ExecutionStatus::QUEUED) queued++;
            else if (report->getStatus() == ExecutionStatus::FILLED) filled++;
            else if (report->getStatus() == ExecutionStatus::PARTIAL_FILLED) partial_filled++;
            else if (report->getStatus() == ExecutionStatus::REJECTED) rejected++;
        }

        std::cout << "Report breakdown:\n";
        std::cout << "  New:      " << queued << "  (added to order book)\n";
        std::cout << "  Pfill:    " << partial_filled << "  (partially filled)\n";
        std::cout << "  Fill:     " << filled << "  (fully executed)\n";
        std::cout << "  Rejected: " << rejected << "  (validation failed)\n";
        std::cout << "\nSuccess! ✓\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n✗ FATAL ERROR: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n✗ UNKNOWN ERROR\n";
        return 1;
    }
}