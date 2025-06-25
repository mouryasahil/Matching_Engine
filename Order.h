#pragma once
#include <chrono>

enum class OrderType { BUY, SELL };

struct Order {
    int id;
    OrderType type;
    int price;
    int quantity;
    std::chrono::high_resolution_clock::time_point timestamp;

    Order() = default;
    Order(int id, OrderType type, int price, int quantity);
};
