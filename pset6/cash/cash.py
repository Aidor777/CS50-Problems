from cs50 import get_float
from math import floor


def main():
    change = get_change()
    nCoins = 0
    cents = round(100 * change)

    nQuarters = floor(cents / 25)
    nCoins += nQuarters
    rest = cents % 25

    nTens = floor(rest / 10)
    nCoins += nTens
    rest = rest % 10

    nFives = floor(rest / 5)
    nCoins += nFives

    # The ones are the remainder of the division by 5
    nCoins += rest % 5

    print(nCoins)


# Prompt user for change amount (positive)
def get_change():
    while True:
        f = get_float("Change owed: ")
        if f > 0:
            break
    return f


main()