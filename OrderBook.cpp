#include "OrderBook.h"
#include <iostream>
#include <algorithm>

using namespace std;

bool CompareBuy::operator()(const Order& a, const Order& b) {
    return a.price < b.price || (a.price == b.price && a.timestamp > b.timestamp);
}

bool CompareSell::operator()(const Order& a, const Order& b) {
    return a.price > b.price || (a.price == b.price && a.timestamp > b.timestamp);
}

OrderBook::OrderBook() {
    ofstream clearFile("trades.txt", ios::trunc);
    clearFile.close();
    tradeLog.open("trades.txt", ios::app);
}

OrderBook::~OrderBook() {
    if (tradeLog.is_open()) tradeLog.close();
}

void OrderBook::placeOrder(OrderType type, int price, int quantity) {
    lock_guard<mutex> lock(bookMutex);
    Order newOrder(++orderId, type, price, quantity);
    activeOrders.emplace(newOrder.id, newOrder);

    cout << "Placing order: ID=" << newOrder.id << ", Type=" << (type == OrderType::BUY ? "BUY" : "SELL")
         << ", Price=" << price << ", Quantity=" << quantity << endl;

    if (type == OrderType::BUY) matchBuyOrder(newOrder);
    else matchSellOrder(newOrder);
}

bool OrderBook::cancelOrder(int id) {
    lock_guard<mutex> lock(bookMutex);
    auto it = activeOrders.find(id);
    if (it != activeOrders.end()) {
        cout << "Cancelling Order ID=" << id << endl;
        tradeLog << "Order Cancelled: ID=" << id << endl;
        activeOrders.erase(it);
        return true;
    }
    cout << "Order ID=" << id << " not found or already executed." << endl;
    return false;
}

void OrderBook::logTrade(const string& direction, int orderId, int matchId, int price, int quantity) {
    if (tradeLog.is_open()) {
        tradeLog << "Trade: " << direction << " OrderID=" << orderId
                 << " matched with " << (direction == "BUY" ? "SELL" : "BUY")
                 << " OrderID=" << matchId << " @ Price=" << price
                 << ", Quantity=" << quantity << endl;
    }
}

void OrderBook::matchBuyOrder(Order& order) {
    while (!sellOrders.empty() && order.quantity > 0) {
        Order topSell = sellOrders.top();
        if (activeOrders.find(topSell.id) == activeOrders.end()) {
            sellOrders.pop();
            continue;
        }
        if (topSell.price > order.price) break;

        sellOrders.pop();
        int tradedQty = min(order.quantity, topSell.quantity);
        cout << "Trade: BUY OrderID=" << order.id << " matched with SELL OrderID=" << topSell.id
             << " @ Price=" << topSell.price << ", Quantity=" << tradedQty << endl;
        logTrade("BUY", order.id, topSell.id, topSell.price, tradedQty);
        order.quantity -= tradedQty;
        topSell.quantity -= tradedQty;
        if (topSell.quantity > 0) sellOrders.push(topSell);
        if (topSell.quantity == 0) activeOrders.erase(topSell.id);
    }
    if (order.quantity > 0) buyOrders.push(order);
    else activeOrders.erase(order.id);
}

void OrderBook::matchSellOrder(Order& order) {
    while (!buyOrders.empty() && order.quantity > 0) {
        Order topBuy = buyOrders.top();
        if (activeOrders.find(topBuy.id) == activeOrders.end()) {
            buyOrders.pop();
            continue;
        }
        if (topBuy.price < order.price) break;

        buyOrders.pop();
        int tradedQty = min(order.quantity, topBuy.quantity);
        cout << "Trade: SELL OrderID=" << order.id << " matched with BUY OrderID=" << topBuy.id
             << " @ Price=" << topBuy.price << ", Quantity=" << tradedQty << endl;
        logTrade("SELL", order.id, topBuy.id, topBuy.price, tradedQty);
        order.quantity -= tradedQty;
        topBuy.quantity -= tradedQty;
        if (topBuy.quantity > 0) buyOrders.push(topBuy);
        if (topBuy.quantity == 0) activeOrders.erase(topBuy.id);
    }
    if (order.quantity > 0) sellOrders.push(order);
    else activeOrders.erase(order.id);
}

bool OrderBook::isOrderBookEmpty() {
    lock_guard<mutex> lock(bookMutex);
    return buyOrders.empty() && sellOrders.empty();
}
