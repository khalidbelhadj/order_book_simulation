#pragma once
#include <cstddef>
#include <list>
#include <queue>

#include "order.h"
#include "order_pool.h"

using Level = std::deque<Order*>;

struct BidComparator {
  bool operator()(const std::pair<size_t, Level*>& lhs,
                  const std::pair<size_t, Level*> rhs) const;
};

struct AskComparator {
  bool operator()(const std::pair<size_t, Level*>& lhs,
                  const std::pair<size_t, Level*> rhs) const;
};

#ifndef VISUAL
// Optimised using a priority queue for performance
using Bids =
    std::priority_queue<std::pair<size_t, Level*>,
                        std::vector<std::pair<size_t, Level*>>, BidComparator>;
using Asks =
    std::priority_queue<std::pair<size_t, Level*>,
                        std::vector<std::pair<size_t, Level*>>, AskComparator>;
#else

// Use a less efficient structure for visualization purposes
template <typename T>
struct VisualQueue {
  std::vector<std::pair<size_t, Level*>> data;
  T comparator;

  void push(const std::pair<size_t, Level*> pair) {
    data.push_back(pair);
    std::sort(data.begin(), data.end(), comparator);
  }

  void pop() { data.pop_back(); }

  std::pair<size_t, Level*> top() { return data.back(); }

  bool empty() { return data.empty(); }

  size_t size() { return data.size(); }
};

using Bids = VisualQueue<BidComparator>;
using Asks = VisualQueue<AskComparator>;

#endif
struct OrderBook {
  OrderPool order_pool;

  size_t next_id;

  std::unordered_map<size_t, Level*> bid_levels;
  Bids bid_prices;

  std::unordered_map<size_t, Level*> ask_levels;
  Asks ask_prices;

  std::unordered_map<size_t, Order*> orders;

  OrderConfirmation add(const size_t price, const size_t quantity,
                        const Side side);
  void cancel(size_t id);
  OrderConfirmation modify(size_t id, size_t new_price, size_t new_quantity,
                           Side new_side);
  std::vector<Trade> match();
};
