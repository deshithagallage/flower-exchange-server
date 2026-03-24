cmake_minimum_required(VERSION 3.10)
project(flower-exchange-mvp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    main.cpp
    engine/Exchange.cpp
    models/Order.cpp
    models/OrderBook.cpp
    models/ExecutionReport.cpp
    services/OrderService.cpp
    utils/CSVReader.cpp
    utils/CSVWriter.cpp
)

# Add executable
add_executable(flower-exchange ${SOURCES})

# Include directories
target_include_directories(flower-exchange PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
