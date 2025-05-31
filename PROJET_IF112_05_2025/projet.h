#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

picture read_picture(const char* filename);
picture decompress_image(const char* filename);
Node* build_tree_from_table(HuffmanTable* table);


picture read_picture(const char* filename) {

    char buff[16];
    picture img;
    FILE *fp;
    int c, rgb_comp_color, width, height;
    //open PPM file for reading
    fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }

    //read image format
    if (!fgets(buff, 3, fp)) {
        perror(filename);
        exit(1);
    }

    //check the image format
    if (buff[0] != 'P' || buff[1] != '6') {
         fprintf(stderr, "Invalid image format (must be 'P6')\n");
         exit(1);
    }
    //read image size information
    if (fscanf(fp, "%d %d %d", &width, &height, &rgb_comp_color) != 3) {
         fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
         exit(1);
    }

        printf("%d %d %d\n", width, height, rgb_comp_color);

    //check rgb component depth
    if (rgb_comp_color!=255) {
         fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
         exit(1);
    }

    fgetc(fp); //'\n';
    //memory allocation for pixel data
    img = new_picture(width, height);

    //read pixel data from file
    if (fread(img.colors, 3 * img.width, img.height, fp) != img.height) {
         fprintf(stderr, "Error loading image '%s'\n", filename);
         exit(1);
    }

    fclose(fp);
    return img;
}



Node* build_tree_from_table(HuffmanTable* table) {

    Node* root  = (Node*)malloc(sizeof(Node));
    root->left  = NULL;
    root->right = NULL;

    for (int i = 0; i < MAX_INTENSITY; i++) {
        if (strlen(table->code[i]) > 0) { // There is an associated code
            Node* current = root;
            for (int j = 0; j < strlen(table->code[i]); j++) {
                if (table->code[i][j] == '0') {
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


picture decompress_image(const char* filename) {

    FILE *in = fopen(filename, "rb");
    
    // Header
    char header[10];
    fscanf(in, "%s", header);
    if (strcmp(header, "P7") != 0) {
        fprintf(stderr, "Format de fichier incorrect\n");
        fclose(in);
        return;
    }
    
    int width, height;
    fscanf(in, "%d %d 255", &width, &height);
    fgetc(in); // Use the newline character
    
    // Image allocation
    picture pic;
    pic.width = width;
    pic.height = height;
    pic.colors = (color*) malloc(width * height * sizeof(color));
    
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
                    pic.colors[index].red = current->value;
                if (c%3==1)
                    pic.colors[index].green = current->value;
                if (c%3==2)
                    pic.colors[index++].blue = current->value;
                c++;
                current = root;
            }
        }
        ind++;
    }
    
    fclose(in);

    return pic;
}

