#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "huffman.h"


void recherche_freq(Node* nodes[], int n, int* min1, int* min2) {
    *min1 = 256;
    *min2 = 255;

    for (int i = 2; i < n; i++) {
        if (nodes[i]->freq < nodes[*min1]->freq) {
            *min2 = *min1;
            *min1 = i;
        } else if (nodes[i]->freq < nodes[*min2]->freq && i != *min1) {
            *min2 = i;
        }
    }
}

Node* build_huffman_tree(float* histogram){
    Node* nodes[256];
    int n_node=0;
    // Création des noeuds dans la liste 
    for(int i=0; i<256; i++){
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

        // On fusionne : on remplace min1 par le nouveau noeud, et on "supprime" min2
        nodes[min1] = new_Node;
        nodes[min2] = nodes[n_node - 1];
        n_node--;
    }
    return nodes[0];
}

void generatehuffmancodes(Node* root, HuffmanTable* table, char* buffer, int depth){ // Fonction récursive 
    if (root->left == NULL && root->right == NULL) {
        buffer[depth] = '\0'; // Fin de chaîne
        strcpy((*table)[(unsigned char)root->value], buffer); // On copie le code dans la table 
    }

    // Branche gauche → ajoute '1'
    if (root->left != NULL) {
        buffer[depth] = '1';
        generatehuffmancodes(root->left, table, buffer, depth + 1);
    }

    // Branche droite → ajoute '0'
    if (root->right != NULL) {
        buffer[depth] = '0';
        generatehuffmancodes(root->right, table, buffer, depth + 1);
    }
}


