#include <stdio.h>
#include <cs50.h>
#include <math.h>

bool followsChecksum(long number);

int main(void)
{
    long cardNumber = get_long("Number: ");
    bool cardTypeFound = false;

    if (followsChecksum(cardNumber))
    {
        // test if American Express
        int AEDigits = cardNumber / pow(10, 13);
        if (AEDigits == 37 || AEDigits == 34)
        {
            cardTypeFound = true;
            printf("AMEX\n");
        }
        // test if Visa
        int visaDigitSixteen = cardNumber / pow(10, 15);
        int visaDigitThirteen = cardNumber / pow(10, 12);
        if (visaDigitSixteen == 4 || visaDigitThirteen == 4)
        {
            cardTypeFound = true;
            printf("VISA\n");
        }
        // test if Mastercard
        int mastercardDigitSixteen = cardNumber / pow(10, 15);
        int mastercardFifteen = (cardNumber / pow(10, 14));
        int mastercardDigitFifteen = mastercardFifteen % 10;
        if (mastercardDigitSixteen == 5 && mastercardDigitFifteen > 0 && mastercardDigitFifteen < 6)
        {
            cardTypeFound = true;
            printf("MASTERCARD\n");
        }
    }

    if (cardTypeFound == false)
    {
        printf("INVALID\n");
    }
}

// determines whether the number follows Luhn's rule
bool followsChecksum(long number)
{
    int powerOfTen = log10(number);
    int checkSum = 0;
    for (int i = 0; i <= powerOfTen; i++)
    {
        long nthPower = number / pow(10, i);
        int nthDigit = nthPower % 10;
        // even i
        if (i % 2 == 0)
        {
            checkSum += nthDigit;
        }
        // odd i
        else
        {
            int tempAddition = 2 * nthDigit;
            checkSum += tempAddition / 10;
            checkSum += tempAddition % 10;
        }
    }

    if (checkSum % 10 == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}