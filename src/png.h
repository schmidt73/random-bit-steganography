#ifndef PNG_HEADER
#define PNG_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <lodepng/lodepng.h>
#include "steganography.h"
#include "random.h"

/* All the following functions return error. 1 being true and 0 being false. */

int init_png_file(const char* inputfile);
int decrypt_png_file();
int encrypt_png_file();

#endif 