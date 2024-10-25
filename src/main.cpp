#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "order_book.h"

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

#include "renderer.h"

#define WIDTH 500
#define HEIGHT 500

int main() {
  using namespace std::chrono;
  renderer::init();

  OrderBook orderbook;

  std::ifstream infile("./data/orders.txt");
  std::string line;

  int trades = 0;
  auto start_time = high_resolution_clock::now();
  while (!renderer::should_close()) {
    renderer::render_state(&orderbook);
    auto current_time = high_resolution_clock::now();

    if (duration_cast<seconds>(current_time - start_time).count() < 0.0001) {
      continue;
    }

    start_time = current_time;

    if (getline(infile, line)) {
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
  }

  infile.close();

  std::cout << "Remaining orders: " << orderbook.orders.size() << std::endl;
  std::cout << "Trades: " << trades << std::endl;
  std::cout << "Total: " << trades + orderbook.orders.size() << std::endl;

  return 0;
}
#endif
