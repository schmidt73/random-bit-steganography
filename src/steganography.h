#ifndef STEGANOGRAPHY_HEADER
#define STEGANOGRAPHY_HEADER

#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "png.h"
#include "random.h"

void print_error(FILE *out, const char *fmt, ...);
void print_success(FILE *out, const char *fmt, ...);
void print_status(FILE *out, const char *fmt, ...);

#endif