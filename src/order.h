#pragma once

#include <cstddef>
#include <vector>

enum Side { BID, ASK };

struct Order {
  size_t id;
  size_t price;
  size_t quantity;
  Side side;
};

struct Trade {
  size_t ask_id;
  size_t bid_id;
  size_t price;
  size_t quantity;
};

struct OrderConfirmation {
  size_t order_id;
  std::vector<Trade> trades;
};
