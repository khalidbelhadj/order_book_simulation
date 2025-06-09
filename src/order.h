#pragma once

#include <cstddef>
#include <vector>

enum class Side { Bid, Ask };

enum class OrderType { Limit, Market };

struct Order {
    size_t id;
    double price;
    double quantity;
    Side side;
    OrderType type = OrderType::Limit;

    Order(size_t id, double price, double quantity, Side side, OrderType type)
        : id(id), price(price), quantity(quantity), side(side), type(type) {}

    static Order market(size_t id, Side side, double quantity) {
        return Order(id, 0, quantity, side, OrderType::Market);
    }

    static Order limit(size_t id, double price, double quantity, Side side) {
        return Order(id, price, quantity, side, OrderType::Limit);
    }
};

struct Trade {
    size_t ask_id;
    size_t bid_id;
    double price;
    double quantity;
    double margin;

    Trade(size_t ask_id, size_t bid_id, double price, double quantity,
          double margin)
        : ask_id(ask_id),
          bid_id(bid_id),
          price(price),
          quantity(quantity),
          margin(margin) {}
};

struct OrderConfirmation {
    size_t order_id;
    std::vector<Trade> trades;

    OrderConfirmation(size_t order_id, std::vector<Trade> trades)
        : order_id(order_id), trades(trades) {}
};
