#include "png.h"

#define PNG_SIGNATURE {137, 80, 78, 71, 13, 10, 26, 10}
#define IHDR_SIGNATURE 'I' + 'H' + 'D' + 'R'
#define IDAT_SIGNATURE 'I' + 'D' + 'A' + 'T'
#define IEND_SIGNATURE 'I' + 'E' + 'N' + 'D'

static unsigned long int width = 0;
static unsigned long int height = 0;
static unsigned long int bit_depth = 0;
static unsigned long int color_type = 0;

static unsigned long int* encryption_key = 0;
static FILE* file = 0;
static unsigned int current_chunksize = 0;

static unsigned char png_signature[] = PNG_SIGNATURE;

struct png_chunk {
  unsigned int data_length;
  unsigned int chunk_id;
  unsigned char *data;
  unsigned int chunk_crc;
};

static int number_of_chunks = 0;
static struct png_chunk* chunks = NULL;

static unsigned char* decode_pixel_data(){
  return 0;
}

static void free_chunks(){
  for(int i = 0; i < number_of_chunks; i++){
    if(chunks[i].data != NULL) free(chunks[i].data);
  }

  number_of_chunks = 0;
  free(chunks);
}

static int load_png_chunks(){
  unsigned char buffer[1024];
  unsigned char* file_data = NULL;

  fseek(file, 0L, SEEK_END);
  int file_size = ftell(file);
  fseek(file, 0L, SEEK_SET);

  file_data = malloc(sizeof(char) * file_size);

  int total_bytes_read = 0;
  int bytes_read = 0;
  while((bytes_read = fread(buffer, sizeof(char), 1024, file)) == 1024){
    memcpy(file_data + total_bytes_read, buffer, bytes_read);
    total_bytes_read += bytes_read;
  }

  if(bytes_read > 0){  
    memcpy(file_data + total_bytes_read, buffer, bytes_read);
    total_bytes_read += bytes_read;
  }

  if(file_size != total_bytes_read){
    free(file_data);
    return 0;
  }
  
  for(int i = 0; i < 8; i++){
    if(png_signature[i] != file_data[i]){
      fprintf(stderr, "couldn't read file\n");
      free(file_data);
      return 0;
    }
  }

  int index = 8;
  int flag = 1;
  while(flag){
    if(index + 12 > file_size){
      flag = 0;
      break;
    }

    struct png_chunk current_chunk;
    current_chunk.data_length = (file_data[index] << 24) + (file_data[index + 1] << 16) + (file_data[index + 2] << 8) + file_data[index + 3];
    index += 4;

    current_chunk.chunk_id = file_data[index] + file_data[index + 1] + file_data[index + 2] + file_data[index + 3];
    index += 4;

    if(index + 4 + current_chunk.data_length > file_size){
      flag = 0;
      break;
    }

    if(current_chunk.data_length > 0){
      unsigned char* chunk_data = malloc(current_chunk.data_length * sizeof(char));
      for(int i = 0; i < current_chunk.data_length; i++){
        chunk_data[i] = file_data[index + i];
      }

      current_chunk.data = chunk_data;
    }else{
      current_chunk.data = NULL;
    }

    index += current_chunk.data_length;

    current_chunk.chunk_crc = (file_data[index] << 24) + (file_data[index + 1] << 16) + (file_data[index + 2] << 8) + file_data[index + 3];
    index += 4;

    chunks = realloc(chunks, sizeof(struct png_chunk) * (number_of_chunks + 1));
    if(memcpy(chunks + number_of_chunks, &current_chunk, sizeof(struct png_chunk)) == NULL && chunks != NULL){
      realloc(chunks, sizeof(struct png_chunk) * (number_of_chunks));
    }else{
      number_of_chunks++;
    }

    if(current_chunk.chunk_id == IEND_SIGNATURE){
      flag = 1;
      break;
    }
  }

  if(flag == 0){
    free(file_data);
    return 0;
  }  

  free(file_data);
  return 1;
}

int init_png_file(const char* inputfile, unsigned long int* key)
{
  size_t readsize = 0;
  unsigned char* buffer = (unsigned char*) malloc(sizeof(char) * 1024);
  encryption_key = key;

  file = fopen(inputfile, "r+b");
  if(file == NULL){
    fprintf(stderr, "couldn't open file\n");
    free_chunks();
    free(buffer);
    return 0;
  }

  if(load_png_chunks() == 0){
    fprintf(stderr, "couldn't open file1\n");
    free_chunks();
    free(buffer);
    return 0;
  }

  if(number_of_chunks < 3){
    fprintf(stderr, "Not complete PNG file\n");
    free_chunks();
    free(buffer);
    return 0;
  }

  int data_section_index = 0;

  width = 0;
  for(int i = 0; i < 4; i++){
    if(i == 3){
      width += chunks->data[data_section_index + i];
    }else{
      width += chunks->data[data_section_index + i];
      width = width << 8;
    }
  }

  data_section_index += 4;

  height = 0;
  for(int i = 0; i < 4; i++){
    if(i == 3){
      height += chunks->data[data_section_index + i];
    }else{
      height += chunks->data[data_section_index + i];
      height = height << 8;
    }
  }

  data_section_index += 4;

  bit_depth = (unsigned long int) chunks->data[data_section_index];
  data_section_index++;
  color_type = (unsigned long int) chunks->data[data_section_index];

  if(height < 1 || width < 1 || height > 2147483647 || width > 2147483647 || bit_depth < 1){
    fprintf(stderr, "invalid width, height, or bit_depth\n");
  }

  free(buffer);
  return 1;
}

int decrypt_png_file()
{
  fprintf(stderr, "File is decryptable. File width: %lu height: %lu bit-depth: %lu color-type: %lu\n", width, height, bit_depth, color_type);

  unsigned char* data = 0;
  unsigned int count = 0; 

  for(int i = 0; i < number_of_chunks; i++){
    if(chunks[i].chunk_id == IDAT_SIGNATURE){
      if(chunks[i].data_length > 0){
        data = realloc(data, chunks[i].data_length + count);
        memcpy(data + count, chunks[i].data, chunks[i].data_length);
        count += chunks[i].data_length;
      }
    }
  }

  unsigned char* inflated_data = 0;
  unsigned long int inflated_data_size = 0;
  unsigned char inflated_data_buffer[1024];

  mz_stream stream;
  memset(&stream, 0, sizeof(stream));

  stream.next_in = data;
  stream.avail_in = (mz_uint32) count;
  stream.next_out = inflated_data_buffer;
  stream.avail_out = (mz_uint32) 1024;

  int status = 0;
  mz_inflateInit(&stream);

  do{
    stream.avail_out = 1024;
    stream.next_out = inflated_data_buffer;
    status = inflate(&stream, Z_NO_FLUSH);
    unsigned int bytes_read = 1024 - stream.avail_out;
    inflated_data_size += bytes_read;
    inflated_data = realloc(inflated_data, inflated_data_size);
    memcpy(inflated_data + inflated_data_size - bytes_read, inflated_data_buffer, bytes_read);
  }while (stream.avail_out == 0);

  free(data);

  if(status != MZ_STREAM_END){
    fprintf(stderr, "failed at decompressing IDAT chunks\n");
    return -1;
  }

  mz_inflateEnd(&stream);

  for(int i = 0; i < inflated_data_size; i++){
    printf("%c", inflated_data[i]);
  }

  fclose(file);
  return 0;
}

int encrypt_png_file()
{

  fclose(file);
  return 0;
}