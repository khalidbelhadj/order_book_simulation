#include <cstddef>
#include <random>

struct OrderGenerator {
  const double FAIR_PRICE = 100.0;
  std::normal_distribution<double> price_dist{FAIR_PRICE, 10};
  std::random_device price_rd{};
  std::mt19937 price_gen{price_rd()};

  const double FAIR_QUANTITY = 100.0;
  std::normal_distribution<double> quantity_dist{FAIR_QUANTITY, 10};
  std::random_device quantity_rd{};
  std::mt19937 quantity_gen{quantity_rd()};

  OrderGenerator() {}

  size_t get_random_price() { return price_dist(price_gen); }

  size_t get_random_quantity() { return quantity_dist(quantity_gen); }

  // int operator*() const { return current; }

  //   PowerOfTwoIterator& operator++() {
  //     current *= 2;
  //     return *this;
  //   }

  //   bool operator!=(const PowerOfTwoIterator& other) const {
  //     return current != other.current;
  //   }
};