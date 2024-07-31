# Orderbook Project

## Overview

The Orderbook Project is a C++ application simulating a financial order book. It manages buy and sell orders, matches them according to their prices, and keeps track of trades. The application supports two types of orders: Good Till Cancel (GTC) and Fill and Kill (FOK).

## Features

- **Order Management:** Add, modify, and cancel orders.
- **Order Matching:** Matches buy and sell orders based on price.
- **Trade Execution:** Executes trades and updates order quantities.
- **Order Types:** Supports Good Till Cancel (GTC) and Fill and Kill (FOK) orders.
- **Orderbook Levels:** Retrieves bid and ask levels with their respective quantities.

### Files

- `include/Order.h`: Header file for the `Order` class and related enums and type definitions.
- `include/Orderbook.h`: Header file for the `Orderbook` class and related type definitions.
- `include/Trade.h`: Header file for the `Trade` class and related structures.
- `src/Order.cpp`: Implementation of `Order` class methods.
- `src/Orderbook.cpp`: Implementation of `Orderbook` class methods.
- `src/Trade.cpp`: Implementation of `Trade` class methods.
- `main.cpp`: Main entry point for the application.

## Getting Started

### Prerequisites

- C++20 compatible compiler (e.g., g++, clang++)
- CMake (if using CMake for building the project)

### Building the Project

#### Using CMake

1. Clone the repository:

   git clone https://github.com/yourusername/orderbook_project.git
   cd orderbook_project
   
## Create a build directory:

mkdir build
cd build

## Configure and build the project:

cmake ..
make

## Run the executable:

./OrderbookProject

## Without CMake

Alternatively, you can compile the project using g++:

g++ -std=c++20 -Iinclude src/Order.cpp src/Trade.cpp src/Orderbook.cpp main.cpp -o orderbook
./orderbook
