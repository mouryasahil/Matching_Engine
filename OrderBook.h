#pragma once
#include "Order.h"
#include <queue>
#include <unordered_map>
#include <fstream>
#include <mutex>

struct CompareBuy {
    bool operator()(const Order& a, const Order& b);
};

struct CompareSell {
    bool operator()(const Order& a, const Order& b);
};

class OrderBook {
public:
    OrderBook();
    ~OrderBook();
    void placeOrder(OrderType type, int price, int quantity);
    bool cancelOrder(int id);
    bool isOrderBookEmpty();

private:
    std::priority_queue<Order, std::vector<Order>, CompareBuy> buyOrders;
    std::priority_queue<Order, std::vector<Order>, CompareSell> sellOrders;
    std::unordered_map<int, Order> activeOrders;
    std::mutex bookMutex;
    int orderId = 0;
    std::ofstream tradeLog;

    void matchBuyOrder(Order& order);
    void matchSellOrder(Order& order);
    void logTrade(const std::string& direction, int orderId, int matchId, int price, int quantity);
};
