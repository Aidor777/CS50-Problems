#include <stdio.h>
#include <cs50.h>
#include <math.h>

int main(void)
{
    float change;
    // prompt user for change amount
    do
    {
        change = get_float("Change owed: ");
    }
    while (change < 0);

    int nCoins = 0;
    int cents = round(change * 100);
    // quarters
    int nQuarters = cents / 25;
    nCoins += nQuarters;
    int rest = cents % 25;
    // tens
    int nTens = rest / 10;
    nCoins += nTens;
    rest = rest % 10;
    // fives
    int nFives = rest / 5;
    nCoins += nFives;
    // the ones are the remainder of the division by 5
    nCoins += rest % 5;

    printf("%i\n", nCoins);
}