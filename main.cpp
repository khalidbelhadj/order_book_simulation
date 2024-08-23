#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <stdlib.h>
#include <thread>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

enum Side {
  BID,
  ASK
};

struct Order {
  size_t id;
  size_t price;
  size_t quantity;
  Side side;
};

struct OrderPool {
    Order *pool = {};
    size_t capacity = 0;
    size_t size = 0;
    deque<Order *> free_orders{};

    OrderPool() {
      capacity = 1000000;
      pool = (Order *)malloc(sizeof(Order) * capacity);
    }

    Order *alloc() {
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

    void free(Order *order) {
      free_orders.push_back(order);
    }
};

struct ThreadPool {
    static const size_t THREAD_COUNT = 10;
    thread threads[THREAD_COUNT];
};

struct Trade {
  size_t ask_id;
  size_t bid_id;
  size_t price;
  size_t quantity;
};

struct OrderConfirmation {
  size_t order_id;
  vector<Trade> trades;
};

using Level = deque<Order *>;

struct BidComparator {
    bool operator()(const pair<size_t, Level*>& lhs, const pair<size_t, Level*> rhs) const {
        return lhs.first < rhs.first;
    }
};

struct AskComparator {
    bool operator()(const pair<size_t, Level*>& lhs, const pair<size_t, Level*> rhs) const {
        return lhs.first > rhs.first;
    }
};

using Bids = priority_queue<pair<size_t, Level*>, vector<pair<size_t, Level*>>, BidComparator>;
using Asks = priority_queue<pair<size_t, Level*>, vector<pair<size_t, Level*>>, AskComparator>;

struct OrderBook {

  OrderPool order_pool;

  size_t next_id;

  // sorted maps of asking and bidding prices
  unordered_map<size_t, Level*> bid_levels;
  Bids bid_prices;

  unordered_map<size_t, Level*> ask_levels;
  Asks ask_prices;

  // id -> order. Owner of order
  // used for canceling and modifying
  unordered_map<size_t, Order *> orders;

  inline OrderConfirmation add(const size_t price, const size_t quantity, const Side side) {
    if (side == BID && bid_levels.find(price) == bid_levels.end()) {
      Level *level = new Level();
      bid_levels[price] = level;
      bid_prices.push({price, level});
    }

    if (side == ASK && ask_levels.find(price) == ask_levels.end()) {
      Level *level = new Level();
      ask_levels[price] = level;
      ask_prices.push({price, level});
    }


    // Order *order = (Order *)calloc(1, sizeof(Order));
    Order *order = order_pool.alloc();
    *order = (Order) {
      .id = next_id,
      .price =  price,
      .quantity = quantity,
      .side = side,
    };

    orders[next_id] = order;
    ++next_id;


    if (side == BID)
        bid_levels[price]->push_back(order);
    if (side == ASK)
        ask_levels[price]->push_back(order);

    return (OrderConfirmation){next_id, match()};
  }

  inline void cancel(size_t id) {
    if (orders.find(id) == orders.end()) return;

    Order * order = orders[id];

    // Remove from levels
    Level *level = order->side == BID ? bid_levels[order->price]: ask_levels[order->price];

    vector<Order *>::iterator it;
    level->erase(remove(level->begin(), level->end(), order), level->end());

    if (level->empty()) {
      if (order->side == BID)
        bid_levels.erase(bid_levels.find(order->price));
      else
        ask_levels.erase(bid_levels.find(order->price));
    }

    orders.erase(id);
  }

  inline OrderConfirmation modify(size_t id, size_t new_price, size_t new_quantity, Side new_side) {
    if (orders.find(id) == orders.end())
      return (OrderConfirmation){id, vector<Trade>()};
    cancel(id);
    return add(new_price, new_quantity, new_side);
  }

  vector<Trade> match() {
    vector<Trade> trades;

    while (!bid_levels.empty() && !ask_levels.empty()) {
      auto bid = bid_prices.top();
      auto ask = ask_prices.top();

      // Asking too much, can't match anymore
      if (bid.first < ask.first) {
        break;
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
        continue;
      }

      auto top_bid = bid.second->front();
      auto top_ask = ask.second->front();
      size_t trade_q = min(top_bid->quantity, top_ask->quantity);

      // We have a trade!
      if (trade_q != 0) {
        top_bid->quantity -= trade_q;
        top_ask->quantity -= trade_q;

        trades.push_back((Trade) {
            .ask_id = top_ask->id,
            .bid_id = bid.first,
            .price = max(bid.first, ask.first),
            .quantity = trade_q
            });
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
    }

    return trades;
  }

};


/*
 * Stress test the Orderbook class for benchmarking
 */
int main() {
    OrderBook orderbook;

    ifstream infile("orders.txt");
    string line;

    int trades = 0;
    int counter = 0;
    while (getline(infile, line)) {
        istringstream iss(line);
        int flag;
        double price;
        int quantity;

        if (!(iss >> flag >> price >> quantity)) {
            cerr << "error" << line << endl;
            continue;
        }

        Side side = (flag == 1) ? BID : ASK;

        auto i = orderbook.add(price, quantity, side);
        trades += i.trades.size();
        counter += 1;
    }

    // Close the file
    infile.close();

    cout << "Remaining orders: " << orderbook.orders.size() << endl;
    cout << "Trades: " << trades << endl;
    cout <<  "Total: " << trades + orderbook.orders.size() << endl;

    return 0;

}
