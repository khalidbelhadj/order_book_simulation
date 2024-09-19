import random

amount = 1_000_000
fair_price = random.randint(100, 1000)
std_dev = 0.5

def generate_price(side):
    price = random.gauss(fair_price, std_dev)

    if side == 1:
        price -= random.uniform(0, 0.1)
    else:
        price += random.uniform(0, 0.1)

    return round(price, 2)



def main():
    with open("./data/orders.txt", "w+") as f:
        for _ in range(amount):
            side = int(random.random() < 0.5)
            price = generate_price(side)
            quantity = int(random.uniform(100, 200))
            f.write(f"{side} {price} {quantity}\n")

if __name__ == "__main__":
    main()
