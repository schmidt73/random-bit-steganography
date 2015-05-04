#include "png.h"

#define PNG_SIGNATURE {137, 80, 78, 71, 13, 10, 26, 10}
#define IHDR_SIGNATURE {'I', 'H', 'D', 'R'}

static unsigned long int width = 0;
static unsigned long int height = 0;
static unsigned long int bit_depth = 0;
static unsigned long int bit_depth = 0;

static unsigned long int* encryption_key = 0;
static FILE* file = 0;
static unsigned int current_chunksize = 0;

static unsigned char png_signature[] = PNG_SIGNATURE;
static unsigned char ihdr_signature[] = IHDR_SIGNATURE;

/* This method will only work if started at the end of a chunk. */
static unsigned char* get_next_chunk(FILE* file){
  unsigned char* buffer = (unsigned char*) malloc(sizeof(char) * 1024);
  unsigned char* chunk = 0;
  fpos_t pos;

  if(fgetpos(file, &pos) != 0){
    fprintf(stderr, "getting file position failed\n");
    free(buffer);
    return chunk;
  }

  for(int i = 0;)

  if(fread(buffer, sizeof(char), 8, file) != 8){
    fprintf(stderr, "couldn't read chunk\n");
    free(buffer);
    return chunk;
  }

  for(int i = 4; i < 8; i++){
    if(buffer[i] < 0x41 || (buffer[i] > 0x5A && buffer[i] < 0x60) || buffer[i] > 0x7A){
        fprintf(stderr, "couldn't read chunk\n");
        free(buffer);
        return chunk;
    }
  }

  unsigned int length = 0;
  for(int i = 0; i < 4; i++){
    if(i == 3){
      length += buffer[i];
    }else{
      length += buffer[i];
      length = length << 8;
    }
  }

  current_chunksize = 12 + length;

  free(buffer);
  fsetpos(file, &pos);
  chunk = (unsigned char*) malloc(sizeof(char) * current_chunksize);
  fread(chunk, sizeof(char), current_chunksize, file);
  return chunk;
}

int init_png_file(const char* inputfile, unsigned long int* key)
{
  size_t readsize = 0;
  unsigned char* buffer = (unsigned char*) malloc(sizeof(char) * 1024);
  encryption_key = key;

  file = fopen(inputfile, "r+b");
  if(file == NULL){
    fprintf(stderr, "couldn't open file\n");
    free(buffer);
    return 0;
  }

  if((readsize = fread(buffer, sizeof(char), 8, file)) != 8){
    fprintf(stderr, "couldn't read files, read failed\n");
    free(buffer);
    return 0;
  }

  for(int i = 0; i < 8; i++){
    if(png_signature[i] != buffer[i]){
      fprintf(stderr, "couldn't read file\n");
      free(buffer);
      return 0;
    }
  }

  unsigned char* ihdr_chunk = get_next_chunk(file);
  int ihdr_flag = 1;

  for(int i = 4; i < 8; i++){
    if(ihdr_chunk[i] != ihdr_signature[i - 4]){
      ihdr_flag = 0;
      break;
    }
  }

  if(current_chunksize < 25 || ihdr_chunk == 0 || ihdr_flag == 0){
    fprintf(stderr, "couldn't find ihdr_chunk\n");
    if(ihdr_chunk != 0) free(ihdr_chunk);
    free(buffer);
    return 0;
  }

  int data_section_index = 8;

  width = 0;
  for(int i = 0; i < 4; i++){
    if(i == 3){
      width += ihdr_chunk[data_section_index + i];
    }else{
      width += ihdr_chunk[data_section_index + i];
      width = width << 8;
    }
  }

  data_section_index += 4;

  height = 0;
  for(int i = 0; i < 4; i++){
    if(i == 3){
      height += ihdr_chunk[data_section_index + i];
    }else{
      height += ihdr_chunk[data_section_index + i];
      height = height << 8;
    }
  }

  if(height < 1 || width < 1 || ){
    fprintf(stderr, "invalid width, height, \n");
  }

  free(ihdr_chunk);
  free(buffer);
  return 1;
}

int decrypt_png_file()
{
  fprintf(stderr, "File is decryptable. File width: %lu height: %lu bit-depth: %lu color-type: %lu\n", width, height, bit-depth, color-type);
  return 0;
}

int encrypt_png_file()
{
  return 0;
}