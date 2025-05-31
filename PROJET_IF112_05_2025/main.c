/*Main.c*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "huffman.h"
#include "ecriture.h"

int main() {
    const char* filename_ppm = "cheese.ppm";
    const char* filename_hppm = "fromage.hppm";

    picture* image = charger(filename_ppm);

    float* histo = histogram_img(*image);
    float ent = entropy(histo);
    for (int i = 0; i < 256; i++) {
        printf(" %3d : %.6f\n", i, histo[i]);
    }
    printf("Entropie de l'image : %f\n", ent);

    compress_img(image, filename_hppm);

    free(histo);
    free(image->pixels);
    free(image);

    return 0;
}
