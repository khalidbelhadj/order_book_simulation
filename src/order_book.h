#pragma once
#include <cstddef>
#include <queue>
#include <vector>

#include "order.h"
#include "order_pool.h"

using Level = std::deque<Order*>;

struct BidComparator {
    bool operator()(const std::pair<double, Level*>& lhs,
                    const std::pair<double, Level*>& rhs) const;
};

struct AskComparator {
    bool operator()(const std::pair<double, Level*>& lhs,
                    const std::pair<double, Level*>& rhs) const;
};

using Item = std::pair<double, Level*>;
using Container = std::vector<Item>;
template <typename T>
using Queue = std::priority_queue<Item, Container, T>;

struct OrderBook {
    OrderPool order_pool;
    size_t next_id = 0;

    std::unordered_map<double, Level*> bid_levels;
    Queue<BidComparator> bid_prices;

    std::unordered_map<double, Level*> ask_levels;
    Queue<AskComparator> ask_prices;

    std::unordered_map<size_t, Order*> orders;

    std::vector<Trade> trades;

    OrderConfirmation create(const double price, const double quantity,
                             const Side side);
    OrderConfirmation add(Order order);
    void cancel(size_t id);
    OrderConfirmation modify(size_t id, double new_price, double new_quantity,
                             Side new_side);
    std::vector<Trade> match();
};
