#ifndef huffman
#define huffman 42

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

typedef char HuffmanTable[256][256];

picture new_pic_HPPM(int width, int height);
void set_pixel(picture pic, int x, int y, color color);
void naive_codes(HuffmanTable table);
picture* charger(const char* filename);
void save_pic_HPPM(picture pic, const char *filename, HuffmanTable table);
void compress_img(picture *img, const char *filename);
float* histogram_img(picture pic);
float entropy(float* histogram);

#endif
