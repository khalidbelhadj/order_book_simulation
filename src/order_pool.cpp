#include "order_pool.h"

OrderPool::OrderPool() {
  capacity = 1000000;
  pool = (Order *)malloc(sizeof(Order) * capacity);
}

Order *OrderPool::alloc() {
  if (!free_orders.empty()) {
    Order *order = free_orders.front();
    free_orders.pop_front();
    return order;
  }

  if (size >= capacity) {
    capacity += 1000000;
    Order *new_order_pool = (Order *)malloc(sizeof(Order) * capacity);
    memcpy(new_order_pool, pool, capacity - 1000000);
    free(pool);
    pool = new_order_pool;
  }

  return &pool[size++];
}

void OrderPool::free(Order *order) { free_orders.push_back(order); }
