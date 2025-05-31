/*Fichier contenant les fonctions de base pour écrire le fichier encodé*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ecriture.h"
#include "huffman.h"

// Crée une nouvelle image HPPM
picture new_pic_HPPM(int width, int height){
    picture pic;
    pic.width = width;
    pic.height = height;
    pic.pixels = (color *)malloc(width * height * sizeof(color));
    return pic;
}

// Définir la couleur d'un pixel
void set_pixel(picture pic, int x, int y, color color) {
    if (x >= 0 && x < pic.width && y >= 0 && y < pic.height) {
        pic.pixels[y * pic.width + x] = color;
    }
}

// Créer une table de codage naïve
void naive_codes(HuffmanTable table) {
    for (int i = 0; i < 256; i++) {
        for (int j = 7; j >= 0; j--) {
            table[i][7 - j] = (i & (1 << j)) ? '1' : '0';
        }
        table[i][8] = '\0';
    }
}

// Charger une image
picture* charger(const char* filename) {
    FILE *file = fopen(filename, "r");

    char type[3];
    fscanf(file, "%2s", type);

    int width, height, maxval;

    fscanf(file, "%d %d %d", &width, &height, &maxval);

    picture* image = malloc(sizeof(picture));
    image->width = width;
    image->height = height;
    image->pixels = malloc(sizeof(color) * width * height);

    for (int i = 0; i < width * height; i++) {
        int r, g, b;
        fscanf(file, "%d %d %d", &r, &g, &b);
        image->pixels[i].red = (unsigned char)r;
        image->pixels[i].green = (unsigned char)g;
        image->pixels[i].blue = (unsigned char)b;
    }

    fclose(file);
    return image;
}

// Sauvegarde une image en HPPM
void save_pic_HPPM(picture pic, const char *filename, HuffmanTable table) {
    FILE *file = fopen(filename, "w");

    fprintf(file, "P7 %d %d 255 ", pic.width, pic.height);

    for (int i = 0; i < 256; i++) {
        fwrite(table[i], sizeof(char), 8, file);
        fputc('\0', file);
    }

    fwrite(pic.pixels, sizeof(color), pic.width * pic.height, file);

    fclose(file);
}


// Calcul de l'histogramme 
float* histogram_img(picture pic) {
    float* histogram = (float*)calloc(256, sizeof(float));
    int total_pixels = pic.width * pic.height;

    for (int i = 0; i < total_pixels; i++) {
        color pixel = pic.pixels[i];
        histogram[pixel.red] += 1;
        histogram[pixel.green] += 1;
        histogram[pixel.blue] += 1;
    }
    float total_values = total_pixels * 3;
    for (int i = 0; i < 256; i++) {
        histogram[i] /= total_values;
    }

    return histogram;
}

//fonction entropie
float entropy(float* histogram) { 
    float E = 0.0;
    for (int i = 0; i < 256; i++) {
        if (histogram[i] > 0) {
            E -= histogram[i] * log2(histogram[i]);
        }
    }
    return E;
}

//Compression d'une image
void compress_img(picture *img, const char *filename) {
    FILE *file = fopen(filename, "wb");

    fprintf(file, "P7 %d %d 255 ", img->width, img->height);

    HuffmanTable table;
    //naive_codes(table);
    char* tab[256];
    float* histo = histogram_img(*img);
    int depth = 0;
    Node* node_0=build_huffman_tree(histo);
    generatehuffmancodes(node_0, &table, *tab, depth);

    for (int i = 0; i < 256; i++) {
        fwrite(table[i], sizeof(char), 8, file);
        fputc('\0', file);
    }

    unsigned char buffer = 0;
    int bit_count = 0;
    for (int i = 0; i < img->width * img->height; i++) {
        unsigned char red = img->pixels[i].red;
        unsigned char green = img->pixels[i].green;
        unsigned char blue = img->pixels[i].blue;

        const char *codes[3] = {table[red], table[green], table[blue]};

        for (int j = 0; j < 3; j++) {
            const char *code = codes[j];
            for (int k = 0; code[k] != '\0'; k++) {
                buffer = (buffer << 1) | (code[k] - '0');
                bit_count++;

                if (bit_count == 8) {
                    fputc(buffer, file);
                    buffer = 0;
                    bit_count = 0;
                }
            }
        }
    }

    if (bit_count > 0) {
        buffer <<= (8 - bit_count);
        fputc(buffer, file);
    }

    fclose(file);
}

