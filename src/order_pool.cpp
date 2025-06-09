#include "order_pool.h"

OrderPool::OrderPool() {
    capacity = ORDER_POOL_SIZE;
    pool = (Order *)malloc(sizeof(Order) * capacity);
}

Order *OrderPool::alloc() {
    if (!free_orders.empty()) {
        Order *order = free_orders.front();
        free_orders.pop_front();
        return order;
    }

    if (size >= capacity) {
        size_t new_capacity = ORDER_POOL_GROWTH_FACTOR * capacity;
        Order *new_order_pool = (Order *)malloc(sizeof(Order) * new_capacity);
        memcpy(new_order_pool, pool, sizeof(Order) * capacity);
        free(pool);
        pool = new_order_pool;
        capacity = new_capacity;
    }

    return &pool[size++];
}

void OrderPool::free(Order *order) { free_orders.push_back(order); }
