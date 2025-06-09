#include "order_generator.h"

#include <cstddef>
#include <random>

#include "order.h"

Order OrderGenerator::generate_order() {
    // Generate a random price around 100 with some noise
    double price = static_cast<double>(100 + price_offset_dist(gen));

    // Generate a random quantity using an exponential distribution
    double quantity = static_cast<double>(qty_dist(gen));

    // Randomly choose side (Bid or Ask)
    Side side = (gen() % 2 == 0) ? Side::Bid : Side::Ask;

    // Create and return the order
    return Order::limit(0, price, quantity, side);
}
