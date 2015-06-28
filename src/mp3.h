#ifndef MP3_HEADER
#define MP3_HEADER

#include <lame/lame.h>
#include <mpg123.h>
#include "steganography.h"

/* All the following functions return error. 1 being true and 0 being false. */

int init_mp3_file(const char* inputfile);
int decrypt_mp3_file();
int encrypt_mp3_file();

#endif