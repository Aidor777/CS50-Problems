#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int alphabetPosition(char c);

int main(int argc, string argv[])
{
    // check that we have 2 command-line args
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");
        return 1;
    }

    // check that the second argument is a key of 26 characters
    string key = argv[1];
    int keyLength = strlen(key);
    if (keyLength != 26)
    {
        printf("Key must contain 26 characters.\n");
        return 1;
    }
    // check that the 26 characters are all letters
    for (int i = 0; i < keyLength; i++)
    {
        char currentMatch = key[i];
        if (!isalpha(currentMatch))
        {
            printf("Key must contain alphabetical characters.\n");
            return 1;
        }
        // check for no duplicates
        int alphabetPositionOfI = alphabetPosition(currentMatch);
        for (int j = i + 1; j < keyLength; j++)
        {
            int alphabetPositionOfJ = alphabetPosition(key[j]);
            if (alphabetPositionOfJ == alphabetPositionOfI)
            {
                printf("Key must match each letter exactly once.\n");
                return 1;
            }
        }
    }

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
            char assignedLetter = key[oldAlphabetPosition - 1];
            // lowercase in the key
            if (islower(assignedLetter))
            {
                int newAlphabetPosition = assignedLetter - 96;
                ciphertext[i] = newAlphabetPosition + 64;
            }
            // uppercase in the key
            else
            {
                int newAlphabetPosition = assignedLetter - 64;
                ciphertext[i] = newAlphabetPosition + 64;
            }
        }
        // lowercase letter
        else if (islower(oldASCIICode))
        {
            int oldAlphabetPosition = oldASCIICode - 96;
            char assignedLetter = key[oldAlphabetPosition - 1];
            // lowercase in the key
            if (islower(assignedLetter))
            {
                int newAlphabetPosition = assignedLetter - 96;
                ciphertext[i] = newAlphabetPosition + 96;
            }
            // uppercase in the key
            else
            {
                int newAlphabetPosition = assignedLetter - 64;
                ciphertext[i] = newAlphabetPosition + 96;
            }
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

// char c must be alphabetical
int alphabetPosition(char c)
{
    if (islower(c))
    {
        return c - 96;
    }
    else
    {
        return c - 64;
    }
}