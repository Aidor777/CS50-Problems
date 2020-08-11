#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int runColemanLiau(string text);

int main(void)
{
    string inputText = get_string("Text: ");

    int grade = runColemanLiau(inputText);

    if (grade < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (grade >= 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", grade);
    }
}

int runColemanLiau(string text)
{
    int nLetters = 0;
    int nWords = 1;
    int nSentences = 0;
    // Only run once through the text
    for (int i = 0, n = strlen(text); i < n; i++)
    {
        // The char is a letter
        if (isalpha(text[i]))
        {
            nLetters++;
        }
        // The char is any kind of space
        else if (isspace(text[i]))
        {
            nWords++;
        }
        // The char denotes end of sentence
        else if (text[i] == '.' || text[i] == '!' || text[i] == '?')
        {
            nSentences++;
        }
    }

    // Calculate params for the formula
    double L = (double) nLetters / nWords * 100;
    double S = (double) nSentences / nWords * 100;
    // Coleman-Liau formula
    double index = 0.0588 * L - 0.296 * S - 15.8;
    return round(index);
}
