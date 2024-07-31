cmake_minimum_required(VERSION 3.10)
project(OrderbookProject)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

# Add GoogleTest
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(OrderbookProject main.cpp src/Order.cpp src/Trade.cpp src/Orderbook.cpp)

# Add test executable
add_executable(OrderbookTest tests/OrderbookTest.cpp src/Order.cpp src/Trade.cpp src/Orderbook.cpp)
target_link_libraries(OrderbookTest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(OrderbookTest)
