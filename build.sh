set -e

clang++ main.cpp -o ./build/main -g -std=c++11 -O3 -Wall -Wextra -pedantic
