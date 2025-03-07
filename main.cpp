#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

#define MAX_TICKERS 1024
#define MAX_ORDERS 10000

enum OrderType { BUY, SELL };

// Order structure
struct Order {
    OrderType type;
    int quantity;
    float price;
    atomic<Order*> next;

    Order(OrderType t, int q, float p) : type(t), quantity(q), price(p), next(nullptr) {}
};

// Lock-free singly linked list for orders
struct OrderList {
    atomic<Order*> head;

    OrderList() : head(nullptr) {}

    void addOrder(OrderType type, int quantity, float price) {
        Order* newOrder = new Order(type, quantity, price);
        newOrder->next.store(head.load(memory_order_relaxed), memory_order_relaxed);

        while (!head.compare_exchange_weak(
            newOrder->next, newOrder, memory_order_release, memory_order_relaxed)) {}
    }

    vector<Order*> toVector() {
        vector<Order*> orders;
        Order* current = head.load(memory_order_acquire);
        while (current) {
            orders.push_back(current);
            current = current->next.load(memory_order_acquire);
        }
        return orders;
    }

    void removeOrder(Order* target) {
        Order* prev = nullptr;
        Order* current = head.load(memory_order_acquire);

        while (current) {
            if (current == target) {
                Order* nextOrder = current->next.load(memory_order_acquire);
                if (prev) {
                    prev->next.store(nextOrder, memory_order_release);
                } else {
                    head.compare_exchange_strong(current, nextOrder, memory_order_release, memory_order_relaxed);
                }
                delete current;
                return;
            }
            prev = current;
            current = current->next.load(memory_order_acquire);
        }
    }
};

// Order book for each ticker
struct TickerOrderBook {
    OrderList buyOrders;
    OrderList sellOrders;
};

// Global ticker array (no maps)
TickerOrderBook tickers[MAX_TICKERS];

// Add order function
void addOrder(OrderType type, int tickerID, int quantity, float price) {
    if (tickerID < 0 || tickerID >= MAX_TICKERS) return;
    if (type == BUY) {
        tickers[tickerID].buyOrders.addOrder(type, quantity, price);
    } else {
        tickers[tickerID].sellOrders.addOrder(type, quantity, price);
    }
}

// Match orders for a ticker
void matchOrders(int tickerID) {
    if (tickerID < 0 || tickerID >= MAX_TICKERS) return;

    auto buyOrders = tickers[tickerID].buyOrders.toVector();
    auto sellOrders = tickers[tickerID].sellOrders.toVector();

    for (auto buyOrder : buyOrders) {
        for (auto sellOrder : sellOrders) {
            if (buyOrder->price >= sellOrder->price) {
                int tradedQuantity = min(buyOrder->quantity, sellOrder->quantity);
                cout << "Matched Ticker " << tickerID
                     << ": " << tradedQuantity
                     << " shares at $" << sellOrder->price << endl;

                buyOrder->quantity -= tradedQuantity;
                sellOrder->quantity -= tradedQuantity;

                if (buyOrder->quantity == 0) {
                    tickers[tickerID].buyOrders.removeOrder(buyOrder);
                }
                if (sellOrder->quantity == 0) {
                    tickers[tickerID].sellOrders.removeOrder(sellOrder);
                }
            }
        }
    }
}

// Random order generator for simulation
void simulateOrders(int threadID, int orderCount) {
    for (int i = 0; i < orderCount; i++) {
        int tickerID = rand() % MAX_TICKERS;
        OrderType type = (rand() % 2 == 0) ? BUY : SELL;
        int quantity = (rand() % 100) + 1;
        float price = (rand() % 1000) / 10.0f + 1.0f;

        addOrder(type, tickerID, quantity, price);
        matchOrders(tickerID);
    }
}

int main() {
    srand(time(nullptr));

    int numThreads = thread::hardware_concurrency();
    vector<thread> threads;

    int ordersPerThread = MAX_ORDERS / numThreads;

    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back(simulateOrders, i, ordersPerThread);
    }

    for (auto& t : threads) {
        t.join();
    }

    cout << "Simulation complete." << endl;

    return 0;
}
