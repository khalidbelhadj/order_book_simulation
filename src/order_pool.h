#pragma once
#include <cstddef>
#include <deque>

#include "order.h"

constexpr size_t ORDER_POOL_SIZE = (1 << 20);
constexpr float ORDER_POOL_GROWTH_FACTOR = 1.5f;

struct OrderPool {
    Order *pool = {};
    size_t capacity = 0;
    size_t size = 0;
    std::deque<Order *> free_orders{};

    OrderPool();
    Order *alloc();
    void free(Order *order);
};
