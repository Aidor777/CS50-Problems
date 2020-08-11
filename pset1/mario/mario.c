#include <stdio.h>
#include <cs50.h>

int main(void)
{
    int n;

    // prompt for positive height
    do
    {
        n = get_int("Height: ");
    }
    while (n < 1 || n > 8);

    for (int i = 0; i < n; i++)
    {
        // print spaces first
        for (int j = 0; j < n - i - 1; j++)
        {
            printf(" ");
        }
        // print hashes
        for (int k = 0; k < i + 1; k++)
        {
            printf("#");
        }
        // print spaces
        printf("  ");
        // print other side hashes (spaces not needed)
        // print hashes
        for (int l = 0; l < i + 1; l++)
        {
            printf("#");
        }
        // newline
        printf("\n");
    }
}