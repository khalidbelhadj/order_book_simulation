#include <cstdio>
#include <fstream>
#include <iostream>

#include "order.h"
#include "order_book.h"
#include "order_generator.h"

int main() {
    OrderBook orderbook;
    OrderGenerator generator;

    std::ifstream infile("./data/orders.txt");
    std::string line;

    int trades = 0;

    for (int i = 0; i < 1000; ++i) {
        Order order = generator.generate_order();
        OrderConfirmation confirmation = orderbook.add(order);
        printf("Order ID: %zu, Price: %zu, Quantity: %zu, Side: %s\n",
               confirmation.order_id, order.price, order.quantity,
               order.side == Side::Bid ? "Bid" : "Ask");
        trades += confirmation.trades.size();
    }

    std::cout << "Remaining orders: " << orderbook.orders.size() << std::endl;
    std::cout << "Trades: " << trades << std::endl;
    std::cout << "Total: " << trades + orderbook.orders.size() << std::endl;
    return 0;
}