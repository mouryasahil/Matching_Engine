// // Simple Low-Latency Order Matching Engine (C++)
// // This is a simplified console-based version simulating core matching logic.

// #include <iostream>
// #include <queue>
// #include <map>
// #include <unordered_map>
// #include <mutex>
// #include <thread>
// #include <vector>
// #include <condition_variable>
// #include <chrono>
// #include <csignal>
// #include <fstream>

// using namespace std;

// volatile sig_atomic_t exitFlag = 0;

// void signalHandler(int signal) {
//     if (signal == SIGINT) {
//         exitFlag = 1;
//     }
// }

// enum class OrderType { BUY, SELL };

// struct Order {
//     int id;
//     OrderType type;
//     int price;
//     int quantity;
//     chrono::high_resolution_clock::time_point timestamp;

//     Order() = default; // Added default constructor

//     Order(int id, OrderType type, int price, int quantity)
//         : id(id), type(type), price(price), quantity(quantity),
//           timestamp(chrono::high_resolution_clock::now()) {}
// };

// struct CompareBuy {
//     bool operator()(const Order& a, const Order& b) {
//         return a.price < b.price || (a.price == b.price && a.timestamp > b.timestamp);
//     }
// };

// struct CompareSell {
//     bool operator()(const Order& a, const Order& b) {
//         return a.price > b.price || (a.price == b.price && a.timestamp > b.timestamp);
//     }
// };

// class OrderBook {
// private:
//     priority_queue<Order, vector<Order>, CompareBuy> buyOrders;
//     priority_queue<Order, vector<Order>, CompareSell> sellOrders;
//     unordered_map<int, Order> activeOrders;
//     mutex bookMutex;
//     int orderId = 0;
//     ofstream tradeLog;

// public:
//     OrderBook() {
//         tradeLog.open("trades.txt", ios::app);
//     }

//     ~OrderBook() {
//         if (tradeLog.is_open()) {
//             tradeLog.close();
//         }
//     }

//     void placeOrder(OrderType type, int price, int quantity) {
//         lock_guard<mutex> lock(bookMutex);
//         Order newOrder(++orderId, type, price, quantity);
//         activeOrders.emplace(newOrder.id, newOrder); // changed to emplace to avoid default-construction issue

//         cout << "Placing order: ID=" << newOrder.id << ", Type=" << (type == OrderType::BUY ? "BUY" : "SELL")
//              << ", Price=" << price << ", Quantity=" << quantity << endl;

//         if (type == OrderType::BUY) {
//             matchBuyOrder(newOrder);
//         } else {
//             matchSellOrder(newOrder);
//         }
//     }

//     bool cancelOrder(int id) {
//         lock_guard<mutex> lock(bookMutex);
//         auto it = activeOrders.find(id);
//         if (it != activeOrders.end()) {
//             cout << "Cancelling Order ID=" << id << endl;
//             tradeLog << "Order Cancelled: ID=" << id << endl;
//             activeOrders.erase(it);
//             return true;
//         }
//         cout << "Order ID=" << id << " not found or already executed." << endl;
//         return false;
//     }

//     void logTrade(const string& direction, int orderId, int matchId, int price, int quantity) {
//         if (tradeLog.is_open()) {
//             tradeLog << "Trade: " << direction << " OrderID=" << orderId
//                      << " matched with " << (direction == "BUY" ? "SELL" : "BUY")
//                      << " OrderID=" << matchId << " @ Price=" << price
//                      << ", Quantity=" << quantity << endl;
//         }
//     }

//     void matchBuyOrder(Order& order) {
//         while (!sellOrders.empty() && order.quantity > 0) {
//             Order topSell = sellOrders.top();
//             if (activeOrders.find(topSell.id) == activeOrders.end()) {
//                 sellOrders.pop();
//                 continue;
//             }
//             if (topSell.price > order.price) break;

//             sellOrders.pop();
//             int tradedQty = min(order.quantity, topSell.quantity);
//             cout << "Trade: BUY OrderID=" << order.id << " matched with SELL OrderID=" << topSell.id
//                  << " @ Price=" << topSell.price << ", Quantity=" << tradedQty << endl;
//             logTrade("BUY", order.id, topSell.id, topSell.price, tradedQty);
//             order.quantity -= tradedQty;
//             topSell.quantity -= tradedQty;
//             if (topSell.quantity > 0) sellOrders.push(topSell);
//             if (topSell.quantity == 0) activeOrders.erase(topSell.id);
//         }
//         if (order.quantity > 0) buyOrders.push(order);
//         else activeOrders.erase(order.id);
//     }

//     void matchSellOrder(Order& order) {
//         while (!buyOrders.empty() && order.quantity > 0) {
//             Order topBuy = buyOrders.top();
//             if (activeOrders.find(topBuy.id) == activeOrders.end()) {
//                 buyOrders.pop();
//                 continue;
//             }
//             if (topBuy.price < order.price) break;

//             buyOrders.pop();
//             int tradedQty = min(order.quantity, topBuy.quantity);
//             cout << "Trade: SELL OrderID=" << order.id << " matched with BUY OrderID=" << topBuy.id
//                  << " @ Price=" << topBuy.price << ", Quantity=" << tradedQty << endl;
//             logTrade("SELL", order.id, topBuy.id, topBuy.price, tradedQty);
//             order.quantity -= tradedQty;
//             topBuy.quantity -= tradedQty;
//             if (topBuy.quantity > 0) buyOrders.push(topBuy);
//             if (topBuy.quantity == 0) activeOrders.erase(topBuy.id);
//         }
//         if (order.quantity > 0) sellOrders.push(order);
//         else activeOrders.erase(order.id);
//     }

//     bool isOrderBookEmpty() {
//         lock_guard<mutex> lock(bookMutex);
//         return buyOrders.empty() && sellOrders.empty();
//     }
// };

// int main() {
//     signal(SIGINT, signalHandler);

//     OrderBook ob;

//     thread t1([&]() {
//         ob.placeOrder(OrderType::BUY, 100, 10);
//         this_thread::sleep_for(chrono::milliseconds(10));
//         ob.placeOrder(OrderType::BUY, 101, 5);
//     });

//     thread t2([&]() {
//         this_thread::sleep_for(chrono::milliseconds(5));
//         ob.placeOrder(OrderType::SELL, 99, 4);
//         ob.placeOrder(OrderType::SELL, 100, 8);
//         this_thread::sleep_for(chrono::milliseconds(10));
//         ob.cancelOrder(4); // Example cancellation
//     });

//     t1.join();
//     t2.join();

//     if (ob.isOrderBookEmpty()) {
//         cout << "Order book is empty." << endl;
//         return 0;
//     } else {
//         cout << "Order book still has orders. Press Ctrl+C to exit." << endl;
//         while (!exitFlag) {
//             this_thread::sleep_for(chrono::milliseconds(100));
//         }
//         cout << "Exit signal received. Shutting down." << endl;
//     }

//     return 0;
// }


#include "OrderBook.h"
#include <iostream>
#include <thread>
#include <csignal>
#include <chrono>

using namespace std;

volatile sig_atomic_t exitFlag = 0;

void signalHandler(int signal) {
    if (signal == SIGINT) {
        exitFlag = 1;
    }
}

int main() {
    signal(SIGINT, signalHandler);

    OrderBook ob;

    thread t1([&]() {
        ob.placeOrder(OrderType::BUY, 100, 10);
        this_thread::sleep_for(chrono::seconds(10));
        ob.placeOrder(OrderType::BUY, 101, 5);
    });

    thread t2([&]() {
        this_thread::sleep_for(chrono::seconds(5));
        ob.placeOrder(OrderType::SELL, 99, 4);
        ob.placeOrder(OrderType::SELL, 100, 8);
        this_thread::sleep_for(chrono::seconds(10));
        ob.cancelOrder(4);
    });

    t1.join();
    t2.join();

    if (ob.isOrderBookEmpty()) {
        cout << "Order book is empty." << endl;
        return 0;
    } else {
        cout << "Order book still has orders. Press Ctrl+C to exit." << endl;
        while (!exitFlag) {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        cout << "Exit signal received. Shutting down." << endl;
    }

    return 0;
}
