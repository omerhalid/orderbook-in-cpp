cmake_minimum_required(VERSION 3.10)
project(OrderbookProject)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(OrderbookProject main.cpp src/Order.cpp src/Trade.cpp src/Orderbook.cpp)
