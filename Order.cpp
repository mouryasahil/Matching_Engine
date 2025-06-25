#include "Order.h"

Order::Order(int id, OrderType type, int price, int quantity)
    : id(id), type(type), price(price), quantity(quantity),
      timestamp(std::chrono::high_resolution_clock::now()) {}
