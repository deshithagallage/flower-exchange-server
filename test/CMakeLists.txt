cmake_minimum_required(VERSION 3.10...3.28)
project(flower-exchange-app_test CXX)

add_executable(${PROJECT_NAME} test_main.cc)

# Note: Tests are not currently configured for the new batch CSV processing architecture.
# To re-enable tests, create new test cases that validate the matching engine behavior
# without the Drogon HTTP framework dependency.
