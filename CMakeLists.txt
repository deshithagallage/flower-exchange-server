cmake_minimum_required(VERSION 3.10)
project(flower-exchange-app CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Drogon
find_package(Drogon CONFIG REQUIRED)

# ===== FLOWER EXCHANGE LIBRARY (NEW) =====
add_library(flower-exchange STATIC
    src/order.cc
    src/order_book_side.cc
    src/order_book.cc
    src/exchange.cc
    src/csv_handler.cc
)
target_include_directories(flower-exchange PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# ===== END FLOWER EXCHANGE LIBRARY =====

# Main web application
add_executable(${PROJECT_NAME} main.cc)

# Link Drogon to main app
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon flower-exchange)

# Include directories for main app
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Auto-include controllers (includes ExchangeCtrl if exists)
aux_source_directory(controllers CTL_SRC)
target_sources(${PROJECT_NAME} PRIVATE ${CTL_SRC})

# ===== CONSOLE BATCH PROCESSOR (NEW - OPTIONAL) =====
add_executable(flower-exchange-console console_main.cc)
target_link_libraries(flower-exchange-console PRIVATE flower-exchange)
target_include_directories(flower-exchange-console PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# ===== END CONSOLE BATCH PROCESSOR =====

message(STATUS "Building Flower Exchange with Drogon backend")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

