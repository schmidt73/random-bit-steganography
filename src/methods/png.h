#ifndef PNG_HEADER
#define PNG_HEADER

#include <stdio.h>
#include "../random.h"
#include <stdlib.h>
#include <string.h>
#include "../libs/miniz.h"

static unsigned char* decode_pixel_data();
static void free_chunks();
static int load_png_chunks();
int init_png_file(const char* inputfile, unsigned long int* key);
int decrypt_png_file();
int encrypt_png_file();

#endif 