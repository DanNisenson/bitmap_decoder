#include <stdio.h>
#include <stdint.h>

#define WIDTH_POS 18
#define PIXELS_POS 54

int cprint(char buffer[], int buflen, int red, int green, int blue)
{
    return snprintf(buffer, buflen, "\033[48;2;%d;%d;%dm \033[0m", red, green, blue);
}

// In BMP pixel rows must be multiple of 4.
// If not, padding will be added and must be accounted for while reading.
int get_padding(int width)
{
    int remainder = width * 3 % 4;
    if (remainder == 0)
    {
        return 0;
    }
    return 4 - remainder;
}

int read_file(char *file)
{
    FILE *f = fopen(file, "rb");
    if (f == NULL)
    {
        printf("Failed to open file: %s\n", file);
        return 1;
    }

    // Check file type
    char file_type[2];
    fread(file_type, sizeof(char), 2, f);
    if (file_type[0] != 'B' || file_type[1] != 'M')
    {
        printf("File is not BMP: %s\n", file);
        return 1;
    }

    // Read width and height
    uint32_t w;
    uint32_t h;
    fseek(f, WIDTH_POS, SEEK_SET);
    fread(&w, sizeof(uint32_t), 1, f);
    fread(&h, sizeof(uint32_t), 1, f);

    // Read pixels
    fseek(f, PIXELS_POS, SEEK_SET);
    uint8_t pixels[h][w][3];
    uint8_t padding = get_padding(w);
    for (uint32_t y = 0; y < h; y++)
    {
        for (uint32_t x = 0; x < w; x++)
        {
            uint8_t b, g, r;
            fread(&b, sizeof(uint8_t), 1, f);
            fread(&g, sizeof(uint8_t), 1, f);
            fread(&r, sizeof(uint8_t), 1, f);
            pixels[y][x][0] = b;
            pixels[y][x][1] = g;
            pixels[y][x][2] = r;
        }
        // Skip padding
        fseek(f, padding, SEEK_CUR);
    }

    // print image
    for (uint32_t y = h - 1; y >= 0; y--)
    {
        for (uint32_t x = 0; x < w; x++)
        {
            uint16_t b = pixels[y][x][0];
            uint16_t g = pixels[y][x][1];
            uint16_t r = pixels[y][x][2];
            char buffer[30];
            cprint(buffer, sizeof(buffer), r, g, b);
            printf("%s", buffer);
        }
        printf("\n");
    }

    fclose(f);
    return 0;
}
