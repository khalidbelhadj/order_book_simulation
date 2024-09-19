set -e

clang++ main.cpp -o ./build/release/main -std=c++17 -O3 -ffast-math -Wall -Wextra -pedantic
