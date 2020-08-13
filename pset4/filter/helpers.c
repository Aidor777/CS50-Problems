#include "helpers.h"
#include <math.h>
#include <stdlib.h>

const int THREE = 3;
const int gx_matrix[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
const int gy_matrix[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

typedef struct
{
    RGBTRIPLE first;
    RGBTRIPLE second;
    RGBTRIPLE last;
}
RGBTRIPLE_row_size3;

typedef struct
{
    int blue_value;
    int green_value;
    int red_value;
}
matrix_computation;

RGBTRIPLE average_pixel(int n_pixels, RGBTRIPLE pixels[n_pixels]);
RGBTRIPLE edge_pixel(RGBTRIPLE_row_size3 matrix[THREE]);
matrix_computation compute_gx(RGBTRIPLE_row_size3 matrix[THREE]);
matrix_computation compute_gy(RGBTRIPLE_row_size3 matrix[THREE]);

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            RGBTRIPLE pixel = image[i][j];
            int average = round((pixel.rgbtBlue + pixel.rgbtGreen + pixel.rgbtRed) / 3.0);
            image[i][j] = (RGBTRIPLE){average, average, average};
        }
    }
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            RGBTRIPLE pixel = image[i][j];
            int sepia_blue = round(0.131 * pixel.rgbtBlue + 0.534 * pixel.rgbtGreen + 0.272 * pixel.rgbtRed);
            if (sepia_blue > 255)
            {
                sepia_blue = 255;
            }
            int sepia_green = round(0.168 * pixel.rgbtBlue + 0.686 * pixel.rgbtGreen + 0.349 * pixel.rgbtRed);
            if (sepia_green > 255)
            {
                sepia_green = 255;
            }
            int sepia_red = round(0.189 * pixel.rgbtBlue + 0.769 * pixel.rgbtGreen + 0.393 * pixel.rgbtRed);
            if (sepia_red > 255)
            {
                sepia_red = 255;
            }
            image[i][j] = (RGBTRIPLE){sepia_blue, sepia_green, sepia_red};
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            RGBTRIPLE temp = image[i][j];
            image[i][j] = image[i][width - j - 1];
            image[i][width - j - 1] = temp;
        }
    }
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE(*image_copy)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image_copy[i][j] = image[i][j];
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // First row
            if (i == 0)
            {
                // Top left corner
                if (j == 0)
                {
                    int n_surrounding_pixels = 4;
                    RGBTRIPLE surrounding_pixels[] = {image_copy[i][j], image_copy[i + 1][j], image_copy[i][j + 1], image_copy[i + 1][j + 1]};
                    image[i][j] = average_pixel(n_surrounding_pixels, surrounding_pixels);
                }
                // Top right corner
                else if (j == width - 1)
                {
                    int n_surrounding_pixels = 4;
                    RGBTRIPLE surrounding_pixels[] = {image_copy[i][j], image_copy[i + 1][j], image_copy[i][j - 1], image_copy[i + 1][j - 1]};
                    image[i][j] = average_pixel(n_surrounding_pixels, surrounding_pixels);
                }
                // Top row
                else
                {
                    int n_surrounding_pixels = 6;
                    RGBTRIPLE surrounding_pixels[] = {image_copy[i][j], image_copy[i][j + 1], image_copy[i][j - 1], image_copy[i + 1][j], image_copy[i + 1][j + 1], image_copy[i + 1][j - 1]};
                    image[i][j] = average_pixel(n_surrounding_pixels, surrounding_pixels);
                }
            }
            // Last row
            else if (i == height - 1)
            {
                // Bottom left corner
                if (j == 0)
                {
                    int n_surrounding_pixels = 4;
                    RGBTRIPLE surrounding_pixels[] = {image_copy[i][j], image_copy[i][j + 1], image_copy[i - 1][j], image_copy[i - 1][j + 1]};
                    image[i][j] = average_pixel(n_surrounding_pixels, surrounding_pixels);
                }
                // Bottom right corner
                else if (j == width - 1)
                {
                    int n_surrounding_pixels = 4;
                    RGBTRIPLE surrounding_pixels[] = {image_copy[i][j], image_copy[i][j - 1], image_copy[i - 1][j], image_copy[i - 1][j - 1]};
                    image[i][j] = average_pixel(n_surrounding_pixels, surrounding_pixels);
                }
                // Bottom row
                else
                {
                    int n_surrounding_pixels = 6;
                    RGBTRIPLE surrounding_pixels[] = {image_copy[i][j], image_copy[i][j + 1], image_copy[i][j - 1], image_copy[i - 1][j], image_copy[i - 1][j + 1], image_copy[i - 1][j - 1]};
                    image[i][j] = average_pixel(n_surrounding_pixels, surrounding_pixels);
                }
            }
            // All other rows
            else
            {
                // Far left pixel
                if (j == 0)
                {
                    int n_surrounding_pixels = 6;
                    RGBTRIPLE surrounding_pixels[] = {image_copy[i][j], image_copy[i][j + 1], image_copy[i + 1][j], image_copy[i + 1][j + 1], image_copy[i - 1][j], image_copy[i - 1][j + 1]};
                    image[i][j] = average_pixel(n_surrounding_pixels, surrounding_pixels);
                }
                // Far right pixel
                else if (j == width - 1)
                {
                    int n_surrounding_pixels = 6;
                    RGBTRIPLE surrounding_pixels[] = {image_copy[i][j], image_copy[i][j - 1], image_copy[i + 1][j], image_copy[i + 1][j - 1], image_copy[i - 1][j], image_copy[i - 1][j - 1]};
                    image[i][j] = average_pixel(n_surrounding_pixels, surrounding_pixels);
                }
                else
                {
                    int n_surrounding_pixels = 9;
                    RGBTRIPLE surrounding_pixels[] = {image_copy[i][j], image_copy[i][j + 1], image_copy[i][j - 1], image_copy[i + 1][j], image_copy[i + 1][j - 1], image_copy[i + 1][j + 1], image_copy[i - 1][j], image_copy[i - 1][j - 1], image_copy[i - 1][j + 1]};
                    image[i][j] = average_pixel(n_surrounding_pixels, surrounding_pixels);
                }
            }
        }
    }

    free(image_copy);
}

// Determine the average pixel from an array of pixels (average of blue, green and red separately)
RGBTRIPLE average_pixel(int n_pixels, RGBTRIPLE pixels[n_pixels])
{
    float blue = 0.0;
    float green = 0.0;
    float red = 0.0;

    for (int i = 0; i < n_pixels; i++)
    {
        RGBTRIPLE current_pixel = pixels[i];
        blue += (float) current_pixel.rgbtBlue / n_pixels;
        green += (float) current_pixel.rgbtGreen / n_pixels;
        red += (float) current_pixel.rgbtRed / n_pixels;
    }

    return (RGBTRIPLE) {round(blue), round(green), round(red)};
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE black_pixel = {0, 0, 0};
    RGBTRIPLE(*image_copy)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image_copy[i][j] = image[i][j];
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // First row
            if (i == 0)
            {
                // Top left corner
                if (j == 0)
                {
                    RGBTRIPLE_row_size3 surrounding_pixels[] = {{black_pixel, black_pixel, black_pixel}, {black_pixel, image_copy[i][j], image_copy[i][j + 1]}, {black_pixel, image_copy[i + 1][j], image_copy[i + 1][j + 1]}};
                    image[i][j] = edge_pixel(surrounding_pixels);
                }
                // Top right corner
                else if (j == width - 1)
                {
                    RGBTRIPLE_row_size3 surrounding_pixels[] = {{black_pixel, black_pixel, black_pixel}, {image_copy[i][j - 1], image_copy[i][j], black_pixel}, {image_copy[i + 1][j - 1], image_copy[i + 1][j], black_pixel}};
                    image[i][j] = edge_pixel(surrounding_pixels);
                }
                // Top row
                else
                {
                    RGBTRIPLE_row_size3 surrounding_pixels[] = {{black_pixel, black_pixel, black_pixel}, {image_copy[i][j - 1], image_copy[i][j], image_copy[i][j + 1]}, {image_copy[i + 1][j - 1], image_copy[i + 1][j], image_copy[i + 1][j + 1]}};
                    image[i][j] = edge_pixel(surrounding_pixels);
                }
            }
            // Last row
            else if (i == height - 1)
            {
                // Bottom left corner
                if (j == 0)
                {
                    RGBTRIPLE_row_size3 surrounding_pixels[] = {{black_pixel, image_copy[i - 1][j], image_copy[i - 1][j + 1]}, {black_pixel, image_copy[i][j], image_copy[i][j + 1]}, {black_pixel, black_pixel, black_pixel}};
                    image[i][j] = edge_pixel(surrounding_pixels);
                }
                // Bottom right corner
                else if (j == width - 1)
                {
                    RGBTRIPLE_row_size3 surrounding_pixels[] = {{image_copy[i - 1][j - 1], image_copy[i - 1][j], black_pixel}, {image_copy[i][j - 1], image_copy[i][j], black_pixel}, {black_pixel, black_pixel, black_pixel}};
                    image[i][j] = edge_pixel(surrounding_pixels);
                }
                // Bottom row
                else
                {
                    RGBTRIPLE_row_size3 surrounding_pixels[] = {{image_copy[i - 1][j - 1], image_copy[i - 1][j], image_copy[i - 1][j + 1]}, {image_copy[i][j - 1], image_copy[i][j], image_copy[i][j + 1]}, {black_pixel, black_pixel, black_pixel}};
                    image[i][j] = edge_pixel(surrounding_pixels);
                }
            }
            // All other rows
            else
            {
                // Far left pixel
                if (j == 0)
                {
                    RGBTRIPLE_row_size3 surrounding_pixels[] = {{black_pixel, image_copy[i - 1][j], image_copy[i - 1][j + 1]}, {black_pixel, image_copy[i][j], image_copy[i][j + 1]}, {black_pixel, image_copy[i + 1][j], image_copy[i + 1][j + 1]}};
                    image[i][j] = edge_pixel(surrounding_pixels);
                }
                // Far right pixel
                else if (j == width - 1)
                {
                    RGBTRIPLE_row_size3 surrounding_pixels[] = {{image_copy[i - 1][j - 1], image_copy[i - 1][j], black_pixel}, {image_copy[i][j - 1], image_copy[i][j], black_pixel}, {image_copy[i + 1][j - 1], image_copy[i + 1][j], black_pixel}};
                    image[i][j] = edge_pixel(surrounding_pixels);
                }
                else
                {
                    RGBTRIPLE_row_size3 surrounding_pixels[] = {{image_copy[i - 1][j - 1], image_copy[i - 1][j], image_copy[i - 1][j + 1]}, {image_copy[i][j - 1], image_copy[i][j], image_copy[i][j + 1]}, {image_copy[i + 1][j - 1], image_copy[i + 1][j], image_copy[i + 1][j + 1]}};
                    image[i][j] = edge_pixel(surrounding_pixels);
                }
            }
        }
    }

    free(image_copy);
}

RGBTRIPLE edge_pixel(RGBTRIPLE_row_size3 matrix[THREE])
{
    matrix_computation gx = compute_gx(matrix);
    matrix_computation gy = compute_gy(matrix);

    // Color byte = sqrt(gx^2 + gy^2)
    int blue = round(sqrt(pow(gx.blue_value, 2) + pow(gy.blue_value, 2)));
    // Max at 255
    if (blue > 255)
    {
        blue = 255;
    }
    int green = round(sqrt(pow(gx.green_value, 2) + pow(gy.green_value, 2)));
    // Max at 255
    if (green > 255)
    {
        green = 255;
    }
    int red = round(sqrt(pow(gx.red_value, 2) + pow(gy.red_value, 2)));
    // Max at 255
    if (red > 255)
    {
        red = 255;
    }
    return (RGBTRIPLE) {blue, green, red};
}

matrix_computation compute_gx(RGBTRIPLE_row_size3 matrix[THREE])
{
    matrix_computation gx = {0, 0, 0};

    for (int i = 0; i < THREE; i++)
    {
        RGBTRIPLE_row_size3 current_row = matrix[i];
        for (int j = 0; j < THREE; j++)
        {
            int matrix_value = gx_matrix[i][j];
            RGBTRIPLE pixel;
            switch (j)
            {
                case 0:
                    pixel = current_row.first;
                    break;

                case 1:
                    pixel = current_row.second;
                    break;

                case 2:
                    pixel = current_row.last;
                    break;
            }

            gx.blue_value += matrix_value * pixel.rgbtBlue;
            gx.green_value += matrix_value * pixel.rgbtGreen;
            gx.red_value += matrix_value * pixel.rgbtRed;
        }
    }

    return gx;
}

matrix_computation compute_gy(RGBTRIPLE_row_size3 matrix[THREE])
{
    matrix_computation gy = {0, 0, 0};

    for (int i = 0; i < THREE; i++)
    {
        RGBTRIPLE_row_size3 current_row = matrix[i];
        for (int j = 0; j < THREE; j++)
        {
            int matrix_value = gy_matrix[i][j];
            RGBTRIPLE pixel;
            switch (j)
            {
                case 0:
                    pixel = current_row.first;
                    break;

                case 1:
                    pixel = current_row.second;
                    break;

                case 2:
                    pixel = current_row.last;
                    break;
            }

            gy.blue_value += matrix_value * pixel.rgbtBlue;
            gy.green_value += matrix_value * pixel.rgbtGreen;
            gy.red_value += matrix_value * pixel.rgbtRed;
        }
    }

    return gy;
}
