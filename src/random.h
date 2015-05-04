#ifndef RANDOM_HEADER
#define RANDOM_HEADER

#include <stdio.h>
#include <stddef.h>

void isaac(unsigned long int buffer[], int length);
void set_key(unsigned long int key[], int length);
void rand_init();

#endif 