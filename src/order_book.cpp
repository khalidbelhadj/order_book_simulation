#include "order_book.h"

#include <cassert>
#include <iostream>

#include "order.h"

bool BidComparator::operator()(const std::pair<double, Level *> &lhs,
                               const std::pair<double, Level *> &rhs) const {
    return lhs.first < rhs.first;
}

bool AskComparator::operator()(const std::pair<double, Level *> &lhs,
                               const std::pair<double, Level *> &rhs) const {
    return lhs.first > rhs.first;
}

OrderConfirmation OrderBook::create(const double price, const double quantity,
                                    const Side side) {
    // Create order
    Order *order = order_pool.alloc();
    // TODO: Handle market orders
    *order = Order::limit(next_id, price, quantity, side);
    orders[next_id] = order;
    ++next_id;

    // Add order to order book
    if (side == Side::Bid) {
        if (bid_levels.find(price) == bid_levels.end()) {
            Level *level = new Level();
            bid_levels[price] = level;
            bid_prices.push({price, level});
        }
        bid_levels[price]->push_back(order);
    }

    if (side == Side::Ask) {
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

OrderConfirmation OrderBook::add(Order order) {
    return create(order.price, order.quantity, order.side);
}

void OrderBook::cancel(size_t id) {
    // Get the order from the map
    auto it = orders.find(id);
    // Order not found, nothing to cancel
    if (it == orders.end()) return;

    Order *order = it->second;

    // Get the level
    Level *level;

    if (order->side == Side::Bid) {
        auto l = bid_levels.find(order->price);
        assert(l != bid_levels.end());
        level = l->second;
    }

    if (order->side == Side::Ask) {
        auto l = ask_levels.find(order->price);
        assert(l != ask_levels.end());
        level = l->second;
    }

    // Remove the order from the level
    level->erase(remove(level->begin(), level->end(), order), level->end());
    orders.erase(id);
}

OrderConfirmation OrderBook::modify(size_t id, double new_price,
                                    double new_quantity, Side new_side) {
    if (orders.find(id) == orders.end())
        return OrderConfirmation(id, std::vector<Trade>());
    cancel(id);
    return create(new_price, new_quantity, new_side);
}

std::vector<Trade> OrderBook::match() {
    std::vector<Trade> new_trades;

    while (true) {
        // No more bids or asks, can't match any more
        if (bid_prices.empty() || ask_prices.empty()) {
            break;
        }

        // Get the top bid and ask
        auto [bid_price, bid_level] = bid_prices.top();
        auto [ask_price, ask_level] = ask_prices.top();

        // Asking too much, can't match anymore
        if (bid_price < ask_price) {
            break;
        }

        // Either being used up, or cancelled and modified
        if (bid_level->empty() || ask_level->empty()) {
            if (bid_level->empty()) {
                bid_prices.pop();
                bid_levels.erase(bid_price);
            }
            if (ask_level->empty()) {
                ask_prices.pop();
                ask_levels.erase(ask_price);
            }
            continue;
        }

        Order *top_bid = bid_level->front();
        Order *top_ask = ask_level->front();

        if (top_bid->price != bid_price) {
            std::cout << "bid price not same: " << top_bid->price << ", "
                      << bid_price << "\n";
            exit(1);
        }

        if (top_ask->price != ask_price) {
            std::cout << "ask price not same: " << top_ask->price << ", "
                      << ask_price << "\n";
            exit(1);
        }

        double trade_q = std::min(top_bid->quantity, top_ask->quantity);
        double trade_p = std::min(top_bid->price, top_ask->price);
        double margin = top_bid->price - top_ask->price;

        // We have a trade!
        if (trade_q != 0) {
            new_trades.push_back(
                Trade(top_ask->id, top_bid->id, trade_p, trade_q, margin));

            // Update quantities
            top_bid->quantity -= trade_q;
            top_ask->quantity -= trade_q;
        }

        // Remove top bid if it's done
        if (top_bid->quantity == 0) {
            bid_level->erase(bid_level->begin());
            orders.erase(top_bid->id);
            order_pool.free(top_bid);
        }

        // Remove top bid if it's done
        if (top_ask->quantity == 0) {
            ask_level->erase(ask_level->begin());
            orders.erase(top_ask->id);
            order_pool.free(top_ask);
        }
    }

    // Add new trades to the order book
    trades.insert(trades.end(), new_trades.begin(), new_trades.end());

    return new_trades;
}
