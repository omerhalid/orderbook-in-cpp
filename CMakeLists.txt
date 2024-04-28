cmake_minimum_required(VERSION 3.16)

# Set the project name
project(OrderBook)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(OrderBook orderbook.cpp)