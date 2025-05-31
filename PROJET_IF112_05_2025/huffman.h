
#include "ecriture.h"

struct Node{
    unsigned char value;
    int freq; 
    struct Node *left, *right;
};
typedef struct Node Node;

void recherche_freq(Node* nodes[], int n, int* min1, int* min2);
Node* build_huffman_tree(float* histogram);
void generatehuffmancodes(Node* root, HuffmanTable* table, char* buffer, int depth);
