#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define WIDTH 256
#define HEIGHT 256

struct color{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};
typedef struct color color;

struct picture{
    int width;
    int height;
    color* pixels;
};
typedef struct picture picture;

struct vector{
    double x1; 
    double x2;
    double y1; 
    double y2;
    struct vector *next;
};
typedef struct vector vector;

struct Node{
    unsigned char value;
    int freq; 
    struct Node *left, *right;
};
typedef struct Node Node;

typedef char HuffmanTable[256][256];


// Creer une nouvelle image HPPM
picture new_pic_HPPM(int width, int height){
    picture pic;
    pic.width = width;
    pic.height = height;
    pic.pixels = (color *)malloc(width * height * sizeof(color));
    return pic;
}

// Sauvegarde une image au format PPM pour tester la decompression
void save_pic_PPM(picture pic, const char *filename) {
    FILE *file = fopen(filename, "w");
    fprintf(file, "P3\n%d %d\n255\n", pic.width, pic.height);
    for (int i = 0; i < pic.width * pic.height; i++) {
        fprintf(file, "%d %d %d\n", pic.pixels[i].red, pic.pixels[i].green, pic.pixels[i].blue);
    }
    fclose(file);
}

// Creer une table de codage naïve
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

    fprintf(file, "P7 %d %d 255 ", pic.width, pic.height); //en-tete du format HPPM

    for (int i = 0; i < 256; i++) { //ecriture de la table de codage
        fwrite(table[i], sizeof(char), 8, file);
        fputc('\0', file);
    }

    fwrite(pic.pixels, sizeof(color), pic.width * pic.height, file); //ecriture des codes binaires de chaque pixel

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
    for (int i = 0; i < 256; i++) {
        histogram[i] /= (total_pixels * 3);
    }

    return histogram;
}

// Fonction entropie
float entropy(float* histogram) { 
    float E = 0.0;
    for (int i = 0; i < 256; i++) {
        if (histogram[i] > 0) {
            E -= histogram[i] * log2(histogram[i]); //formule de l'entropie donnée par l'énoncé
        }
    }
    return E;
}

// Fonction de recherches des 2 plus petitzs fréquences, utiles pour construire l'arbre de Huffman
void recherche_freq(Node* nodes[], int n, int* min1, int* min2) {
    *min1 = 0;
    *min2 = 1;

    for (int i = 2; i < n; i++) {
        if (nodes[i]->freq < nodes[*min1]->freq) {
            *min2 = *min1;
            *min1 = i;
        } else if (nodes[i]->freq < nodes[*min2]->freq && i != *min1) {
            *min2 = i;
        }
    }
}

// Construction de l'arbre de Huffman
Node* build_huffman_tree(float* histogram){
    Node* nodes[256];
    int n_node=0;
    
    for(int i=0; i<256; i++){ // Création des noeuds dans la liste 
        if (histogram[i]!=0){
            Node* new_Node = (Node*)malloc(sizeof(Node));
            new_Node->value= i;
            new_Node->freq= histogram[i];
            new_Node->left = NULL;
            new_Node->right = NULL;
            nodes[n_node++]= new_Node;
        }
    }

    while (n_node > 1) {
        int min1, min2;
        recherche_freq(nodes, n_node, &min1, &min2);

        Node* new_Node = (Node*)malloc(sizeof(Node));
        new_Node->value = min1 + min2;
        new_Node->freq = nodes[min1]->freq + nodes[min2]->freq;
        new_Node->left = nodes[min1];
        new_Node->right = nodes[min2];

        nodes[min1] = new_Node; //on remplace min1 par le nouveau noeud, et on "supprime" min2
        nodes[min2] = nodes[n_node - 1];
        n_node--;
    }
    return nodes[0];
}

// Generer le tableau de Huffman pour la compression d'image
void generatehuffmancodes(Node* root, HuffmanTable* table, char* buffer, int depth){ //fonction récursive 
    if (root->left == NULL && root->right == NULL) {
        buffer[depth] = '\0'; //caractère de fin de chaîne
        strcpy((*table)[(unsigned char)root->value], buffer);
    }

    if (root->left != NULL) { //branche gauche → ajoute '1'
        buffer[depth] = '1';
        generatehuffmancodes(root->left, table, buffer, depth + 1);
    }

    if (root->right != NULL) { //branche droite → ajoute '0'
        buffer[depth] = '0';
        generatehuffmancodes(root->right, table, buffer, depth + 1);
    }
}

// Compression d'une image
void compress_img(picture *img, const char *filename) {
    FILE *file = fopen(filename, "wb");

    fprintf(file, "P7 %d %d 255 ", img->width, img->height); //en-tête format HPPM

    HuffmanTable table; 
    //naive_codes(table);
    char tab[256];
    float* histo = histogram_img(*img);
    int depth = 0;
    Node* node_0=build_huffman_tree(histo); //calcul du premier noeud
    generatehuffmancodes(node_0, &table, tab, depth); //génère la table de Huffman

    for (int i = 0; i < 256; i++) { //ecriture de la table de codage
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

// Fonction utile pour la decompression fournie dans l'enonce du projet
Node* build_tree_from_table(HuffmanTable* table) {

    Node* root  = (Node*)malloc(sizeof(Node));
    root->left  = NULL;
    root->right = NULL;

    for (int i = 0; i < 256; i++) {
        if (strlen((*table)[i]) > 0 > 0) { // There is an associated code
            Node* current = root;
            for (int j = 0; j < strlen((*table)[i]); j++) {
                if (((*table)[i][j]) == '0') {
                    if (!current->left) {
                        current->left = (Node*)malloc(sizeof(Node));
                        current->left->left  = NULL;
                        current->left->right = NULL;
                    }
                    current = current->left;
                } else {
                    if (!current->right) {
                        current->right = (Node*)malloc(sizeof(Node));
                        current->right->left = NULL;
                        current->right->right = NULL;
                    }
                    current = current->right;
                }
            }
            current->value = i; // Set the value to the leaf node
        }
    }
    return root;
}

// Fonction de decompression donnée dans le sujet
picture decompress_image(const char* filename) {
    FILE *in = fopen(filename, "rb");
    
    // Header
    char header[10];
    fscanf(in, "%s", header);
    
    int width, height;
    fscanf(in, "%d %d 255", &width, &height);
    fgetc(in); // Use the newline character
    
    // Image allocation
    picture pic;
    pic.width = width;
    pic.height = height;
    pic.pixels = (color*) malloc(width * height * sizeof(color));
    
    // Coding table reading
    HuffmanTable table;
    fread(&table, sizeof(HuffmanTable), 1, in);

    // Reading and decompression of binary codes
    Node* root = build_tree_from_table(&table);

    Node* current = root;
    unsigned char byte;
    int index = 0;
    int c = 0;
    int ind = 0;
    while (fread(&byte, 1, 1, in) == 1) {
        for (int bit = 7; bit >= 0; bit--) {
            current = (byte & (1 << bit)) ? current->right : current->left;
            
            if (!current->left && !current->right) {
                if (c%3==0)
                    pic.pixels[index].red = current->value;
                if (c%3==1)
                    pic.pixels[index].green = current->value;
                if (c%3==2)
                    pic.pixels[index++].blue = current->value;
                c++;
                current = root;
            }
        }
        ind++;
    }
    
    fclose(in);
    return pic;
}


int main() {
    const char* filename_ppm = "cheese.ppm"; //nom de l'image chargée au format ppm
    const char* filename_hppm = "fromage.hppm"; //nom de l'image compressée au format HPPM

    picture* image = charger(filename_ppm); //chargement image
    float* histo = histogram_img(*image); //calcul de l'histogramme
    float ent = entropy(histo); // calcul de l'entropie
    for (int i = 0; i < 256; i++) {
        printf(" %3d : %.6f\n", i, histo[i]);
    }
    printf("Entropie de l'image : %f\n", ent);

    compress_img(image, filename_hppm); //compression de l'image au format HPPM
    picture decompressed = decompress_image(filename_hppm); //compression de l'image en PPM
    save_pic_PPM(decompressed, "decompressed.ppm"); 
    
    free(decompressed.pixels); //liberation des cases allouées précédemment
    free(histo);
    free(image->pixels);
    free(image);

    return 0;
}