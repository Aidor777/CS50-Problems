from cs50 import get_int
from math import log10
# We could import floor(x) to round up to an integer the result of a division, but // allows for integer division
# Similarly, we could import pow(x, y), but x**y does the same job


def main():
    cardNumber = get_int("Number: ")

    if follows_checksum(cardNumber):
        # Test if American Express
        firstTwoDigitsAE = cardNumber // 10**13
        if firstTwoDigitsAE == 37 or firstTwoDigitsAE == 34:
            print("AMEX")
            return

        # Test if Visa
        visaDigitSixteen = cardNumber // 10**15
        visaDigitThirteen = cardNumber // 10**12
        if visaDigitSixteen == 4 or visaDigitThirteen == 4:
            print("VISA")
            return

        # Test if MasterCard
        mastercardDigitSixteen = cardNumber // 10**15
        mastercardDigitFifteen = cardNumber // 10**14 % 10
        if mastercardDigitSixteen == 5 and mastercardDigitFifteen > 0 and mastercardDigitFifteen < 6:
            print("MASTERCARD")
            return

    print("INVALID")


# Determines whether the number follows Luhn's rule
def follows_checksum(number):
    nDigits = len(str(number))
    checkSum = 0
    for i in range(nDigits):
        # Trick to get the ith digit of the number, indexing from right to left
        ithDigit = number // 10**i % 10

        # even
        if i % 2 == 0:
            checkSum += ithDigit
        # odd
        else:
            temp = 2 * ithDigit
            checkSum += temp // 10
            checkSum += temp % 10

    if checkSum % 10 == 0:
        return True
    else:
        return False


main()