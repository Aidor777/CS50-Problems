// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table (sufficiently high prime number)
const unsigned int N = 43117;

// Hash table
node *table[N];

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    // Determine into which bucket this word would have fallen
    unsigned int bucket_number = hash(word);
    // Go through the linked list in this bucket and try to look for the word
    for (node *tmp = table[bucket_number]; tmp != NULL; tmp = tmp->next)
    {
        if (strcasecmp(word, tmp->word)  == 0)
        {
            return true;
        }
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // Default value is another prime number, about half the size of the hash table
    // Apparently prime number are efficient when computing hash functions so...
    unsigned int hash = 21557;
    for (int i = 0; word[i] != '\0'; i++)
    {
        char c = word[i];
        int code = 0;
        // Same hash for lowercase and uppercase letters
        if (isupper(c))
        {
            code = c - 66;
        }
        else if (islower(c))
        {
            code = c - 98;
        }
        // Try to be as random as possible to spread within the hash table (31 is anoter prime number !)
        hash = (hash * 31 + code) % N;
    }
    return hash;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // First open the dictionary file
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        printf("Could not open %s.\n", dictionary);
        return false;
    }

    int index = 0;
    char word[LENGTH + 1];

    // Read the file, character by character
    for (int c = fgetc(file); c != EOF; c = fgetc(file))
    {
        if (c != '\n')
        {
            // Append character to word
            word[index] = c;
            index++;
        }
        else
        {
            // Finish the word
            word[index] = '\0';
            index = 0;

            // Since we have a complete word we can now store it
            unsigned int bucket_number = hash(word);
            // The first node
            node *bucket = table[bucket_number];
            // If the first node is null, it means the bucket is empty and we can simply add the word there
            if (bucket == NULL)
            {
                node *first_node = malloc(sizeof(node));
                if (first_node == NULL)
                {
                    return false;
                }
                // Have to use string copy to store the word at that node
                strcpy(first_node->word, word);
                first_node->next = NULL;
                table[bucket_number] = first_node;
            }
            // Else we need to iterate over the bucket until we find the last node
            else
            {
                for (node *tmp = table[bucket_number]; tmp != NULL; tmp = tmp->next)
                {
                    if (tmp->next == NULL)
                    {
                        node *new_node = malloc(sizeof(node));
                        if (new_node == NULL)
                        {
                            return false;
                        }
                        strcpy(new_node->word, word);
                        new_node->next = NULL;
                        // tmp is still the previous node, so we can link it to the next one and advance the pointer directly to it
                        tmp->next = new_node;
                        tmp = tmp->next;
                    }
                }
            }
        }
    }
    fclose(file);
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    int n_words = 0;
    // Go through each bucket one by one
    for (int i = 0; i < N; i++)
    {
        for (node *tmp = table[i]; tmp != NULL; tmp = tmp->next)
        {
            n_words++;
        }
    }
    return n_words;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    for (int i = 0; i < N; i++)
    {
        node *tmp = table[i];
        while (tmp != NULL)
        {
            node *next_node = tmp->next;
            free(tmp);
            tmp = next_node;
        }
    }
    return true;
}
