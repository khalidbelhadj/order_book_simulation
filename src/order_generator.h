#include <cstddef>
#include <random>

#include "order.h"

struct OrderGenerator {
    std::default_random_engine gen{std::random_device{}()};
    std::normal_distribution<> price_offset_dist{0.0, 1.5};
    std::exponential_distribution<> qty_dist{1.0 / 5.0};

    Order generate_order();
};