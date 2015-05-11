#ifndef PNG_HEADER
#define PNG_HEADER

#include <stdio.h>
#include "../random.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../libs/lodepng.h"

int init_png_file(const char* inputfile);
int decrypt_png_file();
int encrypt_png_file();

#endif 