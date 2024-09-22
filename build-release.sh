set -e

clang++ src/main.cpp src/order_book.cpp src/order_pool.cpp -o ./build/release/main -std=c++17 -O3 -ffast-math -Wall -Wextra -pedantic
