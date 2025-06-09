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

    size_t order_count = 100000;

    for (size_t i = 0; i < order_count; ++i) {
        Order order = generator.generate_order();
        OrderConfirmation confirmation = orderbook.add(order);
        printf("Order ID: %zu, Price: %f, Quantity: %f, Side: %s\n",
               confirmation.order_id, order.price, order.quantity,
               order.side == Side::Bid ? "Bid" : "Ask");
        trades += confirmation.trades.size();
    }

    std::cout << "Remaining orders: "
              << 100 * (float)orderbook.orders.size() / (float)order_count
              << std::endl;
    std::cout << "Trades: " << trades << std::endl;
    std::cout << "Total: " << trades + orderbook.orders.size() << std::endl;
    size_t margin = 0;
    for (const auto &trade : orderbook.trades) margin += trade.margin;
    std::cout << "Money made: " << margin << std::endl;
    return 0;
}