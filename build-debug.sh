set -e

clang++ src/main.cpp src/order_book.cpp src/order_pool.cpp -o ./build/debug/main -g -std=c++17 -Wall -Wextra -pedantic
