import random
import numpy as np


amount = 1_0000

fair_price = 500
prices = fair_price * np.random.exponential(0.2, amount)
quantities = 100 * np.random.exponential(0.2, amount)


def main():
    with open("./data/orders.txt", "w+") as f:
        for price, quantity in zip(prices, quantities):
            side = int(random.random() < 0.5)
            f.write(f"{side} {price} {quantity}\n")


if __name__ == "__main__":
    main()
