#pragma once

#include <cstddef>
#include <vector>

enum Side { BID, ASK };

struct Order {
  size_t id;
  size_t price;
  size_t quantity;
  Side side;

  Order(size_t id, size_t price, size_t quantity, Side side)
      : id(id), price(price), quantity(quantity), side(side) {}
};

struct Trade {
  size_t ask_id;
  size_t bid_id;
  size_t price;
  size_t quantity;

  Trade(size_t ask_id, size_t bid_id, size_t price, size_t quantity)
      : ask_id(ask_id), bid_id(bid_id), price(price), quantity(quantity) {}
};

struct OrderConfirmation {
  size_t order_id;
  std::vector<Trade> trades;

  OrderConfirmation(size_t order_id, std::vector<Trade> trades)
      : order_id(order_id), trades(trades) {}
};
