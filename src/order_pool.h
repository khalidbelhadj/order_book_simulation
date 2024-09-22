#pragma once
#include <cstddef>
#include <deque>

#include "order.h"

struct OrderPool {
  Order *pool = {};
  size_t capacity = 0;
  size_t size = 0;
  std::deque<Order *> free_orders{};

  OrderPool();
  Order *alloc();
  void free(Order *order);
};
