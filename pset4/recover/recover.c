#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const int BLOCK_SIZE = 512;

typedef uint8_t BYTE;

int is_block_jpg_start(BYTE *ptr);

int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    char *filename = argv[1];

    // Open input file
    FILE *inputfile = fopen(filename, "r");
    if (inputfile == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", filename);
        return 1;
    }

    int n_items_read;
    int jpg_counter = 0;
    FILE *outputfile;
    do
    {
        // Read the memory card block by block
        BYTE *ptr = malloc(BLOCK_SIZE);
        n_items_read = fread(ptr, BLOCK_SIZE, 1, inputfile);
        int is_jpg_start = is_block_jpg_start(ptr);

        if (is_jpg_start == 1)
        {
            // First close previously opened file, if any
            if (jpg_counter > 0)
            {
                fclose(outputfile);
            }

            // We are a the start of a JPG file, so we open a new one
            // First format the name to ###.jpg
            char *jpg_filename = malloc(8);
            sprintf(jpg_filename, "%i.jpg", jpg_counter);
            if (jpg_counter < 10)
            {
                sprintf(jpg_filename, "00%i.jpg", jpg_counter);
            }
            else
            {
                sprintf(jpg_filename, "0%i.jpg", jpg_counter);
            }
            outputfile = fopen(jpg_filename, "w");
            if (outputfile == NULL)
            {
                fclose(inputfile);
                fprintf(stderr, "Could not create %s.\n", jpg_filename);
                return 1;
            }

            // Write the first block into the file
            fwrite(ptr, BLOCK_SIZE, 1, outputfile);
            jpg_counter ++;
            free(jpg_filename);
        }
        // Continue to write in the open file, provided we opened at least one and we didn't reach the end of the input file
        else if (jpg_counter > 0 && n_items_read == 1)
        {
            fwrite(ptr, BLOCK_SIZE, 1, outputfile);
        }
        // Free the memory
        free(ptr);
    }
    // As long as we don't reach the end of the file
    while (n_items_read == 1);

    // Close input file
    fclose(inputfile);

    // Close last file
    fclose(outputfile);
}

// Determine whether this block could be the start of a JPG file
int is_block_jpg_start(BYTE *ptr)
{
    if (ptr[0] != 0xff || ptr[1] != 0xd8 || ptr[2] != 0xff || !(ptr[3] >= 0xe0 && ptr[3] <= 0xef))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
