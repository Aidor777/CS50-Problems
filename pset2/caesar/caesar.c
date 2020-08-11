#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int main(int argc, string argv[])
{
    // check that we have 2 command-line args
    if (argc != 2)
    {
        printf("Usage: ./caesar key\n");
        return 1;
    }

    // check that the second argument is a sequence of numbers
    string keyAsString = argv[1];
    for (int i = 0, n = strlen(keyAsString); i < n; i++)
    {
        if (!isdigit(keyAsString[i]))
        {
            printf("Usage: ./caesar key\n");
            return 1;
        }
    }
    // we can then convert it to int
    int key = atoi(argv[1]);

    string plaintext = get_string("plaintext: ");

    // Init empty string of required size
    int textLength = strlen(plaintext);
    char ciphertext[textLength];
    ciphertext[0] = '\0';
    for (int i = 0; i < textLength; i++)
    {
        int oldASCIICode = plaintext[i];
        // uppercase letter
        if (isupper(oldASCIICode))
        {
            int oldAlphabetPosition = oldASCIICode - 64;
            int newAlphabetPosition = (oldAlphabetPosition + key) % 26;
            ciphertext[i] = newAlphabetPosition + 64;
        }
        // lowercase letter
        else if (islower(oldASCIICode))
        {
            int oldAlphabetPosition = oldASCIICode - 96;
            int newAlphabetPosition = (oldAlphabetPosition + key) % 26;
            ciphertext[i] = newAlphabetPosition + 96;
        }
        // else keep the character as was
        else
        {
            ciphertext[i] = plaintext[i];
        }
    }
    // End the string
    ciphertext[textLength] = '\0';

    printf("ciphertext: %s\n", ciphertext);
    return 0;
}