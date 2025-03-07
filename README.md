# Stock Trading Engine

This project simulates a simple stock trading engine where buy and sell orders are matched based on price and quantity. It uses a lock-free linked list to store orders and performs order matching in multiple threads to simulate real-time stock trading.

## Features
- **Order Matching:** Orders are matched based on price and quantity.
- **Multi-threading:** Simulates multiple threads handling orders concurrently.
- **Lock-free Order Storage:** Orders are stored using a lock-free linked list.
- **Order Types:** Supports both `BUY` and `SELL` orders.
- **Order Simulation:** Randomized order generation for simulation.

## Requirements
- C++11 or higher
- Compiler supporting C++11 (e.g., GCC, Clang)
- (Optional) `valgrind` or another memory checking tool for debugging memory issues

## Setup and Installation

### 1. Clone the repository (if not already done)
```bash
git clone https://github.com/rbhard10/stock-trading-engine.git
cd stock-trading-engine
```

### 2. Compile the code
To compile the code, use `g++` (or any C++11-compatible compiler):

```bash
g++ -std=c++11 -o trading_engine trading_engine.cpp -pthread
```

### 3. Run the program
Once compiled, run the program using:

```bash
./trading_engine
```

The program will simulate stock trading and print matched orders.

### 4. Debugging with Valgrind (optional)
To check for memory issues, you can use `valgrind`:

```bash
valgrind ./trading_engine
```

## How It Works

### Data Structures:
- **Order:** Each order contains the order type (`BUY` or `SELL`), quantity, price, and a pointer to the next order.
- **OrderList:** A lock-free, atomic linked list that stores orders for each ticker.
- **TickerOrderBook:** Stores two `OrderList` instances: one for `BUY` orders and one for `SELL` orders.
  
### Order Matching:
1. **Add Order:** Orders are added to the appropriate list based on type (`BUY` or `SELL`).
2. **Match Orders:** Buy and sell orders are matched based on price and quantity. If a match occurs, the order is removed from the list.

### Multi-threading:
The program simulates multiple threads generating orders concurrently and matching them in real-time.

## Contributing

If you'd like to contribute, feel free to fork the repository and submit a pull request. Issues and feature requests are welcome.

