#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "order_book.h"

// #define VISUAL

#ifndef VISUAL
int main() {
  OrderBook orderbook;

  std::ifstream infile("./data/orders.txt");
  std::string line;

  int trades = 0;
  while (getline(infile, line)) {
    std::istringstream iss(line);
    int flag;
    double price;
    int quantity;

    if (!(iss >> flag >> price >> quantity)) {
      std::cerr << "error" << line << std::endl;
      continue;
    }

    Side side = (flag == 1) ? BID : ASK;

    auto i = orderbook.add(price, quantity, side);
    trades += i.trades.size();
  }

  infile.close();

  std::cout << "Remaining orders: " << orderbook.orders.size() << std::endl;
  std::cout << "Trades: " << trades << std::endl;
  std::cout << "Total: " << trades + orderbook.orders.size() << std::endl;
  return 0;
}

#else
#include <algorithm>
#include <chrono>
#include <thread>

#include "raylib.h"

#define WIDTH 500
#define HEIGHT 500

int main() {
  InitWindow(WIDTH, HEIGHT, "Order Book");

  OrderBook orderbook;

  std::ifstream infile("./data/orders.txt");
  std::string line;

  int trades = 0;
  while (!WindowShouldClose() && getline(infile, line)) {
    std::istringstream iss(line);
    int flag;
    double price;
    int quantity;

    if (!(iss >> flag >> price >> quantity)) {
      std::cerr << "error" << line << std::endl;
      continue;
    }

    Side side = (flag == 1) ? BID : ASK;

    BeginDrawing();
    {
      ClearBackground(WHITE);

      for (size_t i = 0; i < std::min((size_t)5, orderbook.bid_prices.size());
           ++i) {
        DrawText(
            TextFormat("Bid %d, Size %zu", orderbook.bid_prices.top().first,
                       orderbook.bid_prices.top().second->size()),
            10, 10 + 20 * i, 20, BLACK);
        orderbook.bid_prices.pop();
      }

      for (size_t i = 0; i < std::min((size_t)5, orderbook.ask_prices.size());
           ++i) {
        DrawText(
            TextFormat("Ask %d, Size %zu", orderbook.ask_prices.top().first,
                       orderbook.ask_prices.top().second->size()),
            10, HEIGHT - (10 + 20 * i), 20, BLACK);
        orderbook.ask_prices.pop();
      }
    }
    EndDrawing();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    auto i = orderbook.add(price, quantity, side);
    trades += i.trades.size();
  }

  infile.close();

  std::cout << "Remaining orders: " << orderbook.orders.size() << std::endl;
  std::cout << "Trades: " << trades << std::endl;
  std::cout << "Total: " << trades + orderbook.orders.size() << std::endl;

  return 0;
}
#endif