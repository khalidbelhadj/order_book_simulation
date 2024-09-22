#include <fstream>
#include <iostream>
#include <sstream>

#include "order_book.h"

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