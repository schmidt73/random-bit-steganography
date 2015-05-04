#ifndef PNG_HEADER
#define PNG_HEADER

#include <stdio.h>
#include "../random.h"
#include <stdlib.h>

/* Returns 0 on failure and 1 on success */
int init_png_file(const char* inputfile, unsigned long int* key);
int decrypt_png_file();
int encrypt_png_file();

#endif 