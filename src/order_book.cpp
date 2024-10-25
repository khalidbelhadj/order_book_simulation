#include "order_book.h"

#include <chrono>
#include <iostream>
#include <thread>

bool BidComparator::operator()(const std::pair<size_t, Level *> &lhs,
                               const std::pair<size_t, Level *> rhs) const {
  return lhs.first < rhs.first;
}

bool AskComparator::operator()(const std::pair<size_t, Level *> &lhs,
                               const std::pair<size_t, Level *> rhs) const {
  return lhs.first > rhs.first;
}

OrderConfirmation OrderBook::add(const size_t price, const size_t quantity,
                                 const Side side) {
  // Create order
  Order *order = order_pool.alloc();
  *order = Order(next_id, price, quantity, side);
  orders[next_id] = order;
  ++next_id;

  // Add order to order book
  if (side == BID) {
    if (bid_levels.find(price) == bid_levels.end()) {
      Level *level = new Level();
      bid_levels[price] = level;
      bid_prices.push({price, level});
    }
    bid_levels[price]->push_back(order);
  }

  if (side == ASK) {
    if (ask_levels.find(price) == ask_levels.end()) {
      Level *level = new Level();
      ask_levels[price] = level;
      ask_prices.push({price, level});
    }
    ask_levels[price]->push_back(order);
  }

  // Try to match
  return OrderConfirmation(next_id, match());
}

void OrderBook::cancel(size_t id) {
  if (orders.find(id) == orders.end()) return;

  Order *order = orders[id];

  // Remove from levels
  Level *level =
      order->side == BID ? bid_levels[order->price] : ask_levels[order->price];

  level->erase(remove(level->begin(), level->end(), order), level->end());

  if (level->empty()) {
    if (order->side == BID)
      bid_levels.erase(bid_levels.find(order->price));
    else
      ask_levels.erase(bid_levels.find(order->price));
  }

  orders.erase(id);
}

OrderConfirmation OrderBook::modify(size_t id, size_t new_price,
                                    size_t new_quantity, Side new_side) {
  if (orders.find(id) == orders.end())
    return OrderConfirmation(id, std::vector<Trade>());
  cancel(id);
  return add(new_price, new_quantity, new_side);
}

std::vector<Trade> OrderBook::match() {
  std::vector<Trade> trades;

  while (!bid_levels.empty() && !ask_levels.empty()) {
    auto bid = bid_prices.top();
    auto ask = ask_prices.top();

    // Asking too much, can't match anymore
    if (bid.first < ask.first) {
      break;
    }

    // TODO: Not sure if this is needed
    // Either being used up, or cancelled and modified
    if (bid.second->empty() || ask.second->empty()) {
      if (bid.second->empty()) {
        bid_prices.pop();
        bid_levels.erase(bid.first);
        delete bid.second;
      }
      if (ask.second->empty()) {
        ask_prices.pop();
        ask_levels.erase(ask.first);
        delete ask.second;
      }
      continue;
    }

    auto top_bid = bid.second->front();
    auto top_ask = ask.second->front();
    size_t trade_q = std::min(top_bid->quantity, top_ask->quantity);

    // We have a trade!
    if (trade_q != 0) {
      top_bid->quantity -= trade_q;
      top_ask->quantity -= trade_q;

      trades.push_back(Trade(top_ask->id, bid.first,
                             std::min(bid.first, ask.first), trade_q));
    }

    // Remove top bid if it's done
    if (top_bid->quantity == 0) {
      bid.second->erase(bid.second->begin());
      orders.erase(top_bid->id);
      order_pool.free(top_bid);
    }

    // Remove top bid if it's done
    if (top_ask->quantity == 0) {
      ask.second->erase(ask.second->begin());
      orders.erase(top_ask->id);
      order_pool.free(top_ask);
    }

    // Either being used up, or cancelled and modified
    if (bid.second->empty() || ask.second->empty()) {
      if (bid.second->empty()) {
        bid_prices.pop();
        bid_levels.erase(bid.first);
        delete bid.second;
      }
      if (ask.second->empty()) {
        ask_prices.pop();
        ask_levels.erase(ask.first);
        delete ask.second;
      }
    }
  }

  return trades;
}
