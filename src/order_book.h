#pragma once
#include <cstddef>
#include <queue>

#include "order.h"
#include "order_pool.h"

using Level = std::deque<Order*>;

struct BidComparator {
    bool operator()(const std::pair<size_t, Level*>& lhs,
                    const std::pair<size_t, Level*>& rhs) const;
};

struct AskComparator {
    bool operator()(const std::pair<size_t, Level*>& lhs,
                    const std::pair<size_t, Level*>& rhs) const;
};

using Item = std::pair<size_t, Level*>;
using Container = std::vector<Item>;
template <typename T>
using Queue = std::priority_queue<Item, Container, T>;

struct OrderBook {
    OrderPool order_pool;

    size_t next_id;

    std::unordered_map<size_t, Level*> bid_levels;
    Queue<BidComparator> bid_prices;

    std::unordered_map<size_t, Level*> ask_levels;
    Queue<AskComparator> ask_prices;

    std::unordered_map<size_t, Order*> orders;

    OrderConfirmation create(const size_t price, const size_t quantity,
                             const Side side);
    OrderConfirmation add(Order order) {
        return create(order.price, order.quantity, order.side);
    }
    void cancel(size_t id);
    OrderConfirmation modify(size_t id, size_t new_price, size_t new_quantity,
                             Side new_side);
    std::vector<Trade> match();
};
