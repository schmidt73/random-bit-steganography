#ifndef STEGANOGRAPHY_HEADER
#define STEGANOGRAPHY_HEADER

#include "random.h"
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "methods/png.h"

const char* get_filename_ext(const char *filename);
void print_version();
void print_usage();
int init_decrypting_data(const char* type, const char* inputfile);
int init_encrypting_data(const char* type, const char* inputfile);

#endif