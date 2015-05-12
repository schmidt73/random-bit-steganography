#include "png.h"

static const char* file_name = 0;
static unsigned char* decoded_image_data = 0;
static unsigned int height = 0;
static unsigned int width = 0;
static size_t png_size = 0;
static LodePNGState state;

int init_png_file(const char* input_file)
{
  unsigned int error = 0;
  unsigned char* encoded_image_data = 0;
  file_name = input_file;

  lodepng_state_init(&state);
  error = lodepng_load_file(&encoded_image_data, &png_size, input_file);

  if(error){
    fprintf(stderr, "couldn't load file\n");
    lodepng_state_cleanup(&state);
    return 1;
  }

  error = lodepng_inspect(&width, &height, &state, encoded_image_data, png_size);

  if(error){
    fprintf(stderr, "couldn't get png info\n");
    lodepng_state_cleanup(&state);
    return 1;
  }

  state.info_raw = state.info_png.color;
  state.encoder.auto_convert = 0;

  error = lodepng_decode(&decoded_image_data, &width, &height, &state, encoded_image_data, png_size);
  free(encoded_image_data);

  if(error || decoded_image_data == 0){
    fprintf(stderr, "couldn't decode file\n");
    lodepng_state_cleanup(&state);
    free(decoded_image_data);
    return 1;
  }

  return 0;
}

int decrypt_png_file()
{
  unsigned long int first_isaac_buffer[256];
  unsigned long int isaac_buffer[256];
  isaac(first_isaac_buffer, 256);
  isaac(isaac_buffer, 256);

  int bitdepth = state.info_raw.bitdepth;
  int bits_per_pixel = -1;
  switch(state.info_raw.colortype){
    case LCT_GREY:
      if(bitdepth == 1 || bitdepth == 2 || bitdepth == 4 || bitdepth == 8 || bitdepth == 16){
        bits_per_pixel = bitdepth;
      }

      break;
    case LCT_RGB:
      if(bitdepth == 8 || bitdepth == 16){
        bits_per_pixel = 3 * bitdepth;
      }
      break;
    case LCT_PALETTE:
      if(bitdepth == 1 || bitdepth == 2 || bitdepth == 4 || bitdepth == 8){
        bits_per_pixel = bitdepth;
      }
      break;
    case LCT_GREY_ALPHA:
      if(bitdepth == 8 || bitdepth == 16){
        bits_per_pixel = bitdepth * 2;
      }
      break;
    case LCT_RGBA:
      if(bitdepth == 8 || bitdepth == 16){
        bits_per_pixel = 4 * bitdepth;
      }
      break;
    default:
      break;
  }

  lodepng_state_cleanup(&state);

  if(bits_per_pixel == -1){
    fprintf(stderr, "invalid bitdepth and color type\n");
    free(decoded_image_data);
    return 1;
  }

  uint32_t number_of_bytes = 0;
  if(bits_per_pixel % 8 == 0){
    int bytes_per_pixel = bits_per_pixel / 8;
    for(int x = 0; x < (sizeof(uint32_t) * 8); x++){
      number_of_bytes = number_of_bytes + ((first_isaac_buffer[x] & 1) ^ (decoded_image_data[bytes_per_pixel * x] & 1));
      if(x != (sizeof(uint32_t) * 8) - 1) number_of_bytes = number_of_bytes << 1; 
    }
  }

  if(number_of_bytes == 0){
    fprintf(stderr, "couldn't read number_of_bytes\n");
    free(decoded_image_data);
    return 1;
  }

  unsigned int block_area = number_of_bytes * 8;

  if(block_area > (width * height) - (sizeof(uint32_t) * 8)){
    fprintf(stderr, "invalid number of bytes for image size\n");
    free(decoded_image_data);
    return 1;
  }

  unsigned int height_mask = 1;
  while(height_mask < height){
    height_mask = (height_mask << 1) + 1;
  }

  unsigned int width_mask = 1;
  while(width_mask < width){
    width_mask = (width_mask << 1) + 1;
  }

  unsigned int block_width = 0, block_height = 0, block_x = 0, block_y = 0;

  int index = 0;
  int count = 0;
  while((block_width * block_height) < block_area || block_width > width || block_height > (height - 1)){
    if(count < 1000){
      if(index + 1 > 255){
        isaac(isaac_buffer, 256);
        index = 0;
      }

      block_height = isaac_buffer[index] & height_mask;
      block_width = isaac_buffer[index + 1] & width_mask;
      index += 2;
    }else{
      if(block_height > (height - 1)) block_height = height - 2;
      if(block_width > width) block_width = width - 1;
      if((block_width * block_height) < block_area){
        block_width = width;
        block_height = height - 1;
      }

      break;
    }
    count++;
  }

  count = 0;
  do{
    if(count < 1000){
      if(index + 1> 255){
        isaac(isaac_buffer, 256);
        index = 0;
      }

      block_x = isaac_buffer[index] & width_mask;
      block_y = isaac_buffer[index + 1] & height_mask;
      index += 2;
    }else{
      if(block_y + block_height > height) block_y = 1;
      if(block_x + block_width > width) block_x = 0;
      if(block_y < 1) block_y = 1;
      break;
    }
    
    count++; 
  }while((block_x + block_width > width) || (block_y + block_height > (height - 1)) || block_y < 1);

  if(bits_per_pixel % 8 == 0){
    int bytes_per_pixel = bits_per_pixel / 8;
    index = 0;

    int rand_count = 0;
    unsigned char current_char = 0;
    int pixel_index = 0;
    for(int x = 0; x < block_width; x++){
      if(index > number_of_bytes * 8 - 1) break;
      for(int y = 0; y < block_height; y++){
        if(index > number_of_bytes * 8 - 1) break;
        if(rand_count > 255){
          rand_count = 0;
          isaac(isaac_buffer, 256);
        }

        if(bytes_per_pixel > 1) pixel_index = isaac_buffer[rand_count] % (bytes_per_pixel - 1);
        int bit = ((1 & isaac_buffer[rand_count]) ^ (decoded_image_data[bytes_per_pixel * (y + block_y) * width + bytes_per_pixel * (x + block_x) + pixel_index] & 1));

        if(index % 8 == 0 && index != 0){
          fwrite(&current_char, sizeof(char), 1, stdout);
          fflush(stdout);
          current_char = 0;
        }

        current_char += bit;
        if(index % ((8 * ((index / 8) + 1)) - 1) != 0 || index == 0) current_char = current_char << 1;

        rand_count++;
        index++;
      }
    }

    fwrite(&current_char, sizeof(char), 1, stdout);
    fflush(stdout);
  }

  free(decoded_image_data);
  return 0;
}

int encrypt_png_file()
{
  unsigned int block_width = 0, block_height = 0, block_x = 0, block_y = 0;

  unsigned long int first_isaac_buffer[256];
  unsigned long int isaac_buffer[256];
  isaac(first_isaac_buffer, 256);
  isaac(isaac_buffer, 256);

  unsigned char in_buffer[1024];
  unsigned char* in_bytes = 0;
  uint32_t n_in_bytes = 0;

  unsigned int count = 0;
  while((count = fread(in_buffer, sizeof(char), 1024, stdin)) != 0){
    if(in_bytes == NULL){
      in_bytes = malloc(n_in_bytes + count);
    }else{
      in_bytes = realloc(in_bytes, n_in_bytes + count);
    }

    memcpy(in_bytes + n_in_bytes, in_buffer, count);
    n_in_bytes += count;
    if(count != 1024){
      break;
    }
  }

  
  if(in_bytes == NULL){
    fprintf(stderr, "read failed\n");
    lodepng_state_cleanup(&state);
    free(decoded_image_data);
    return 1;
  }

  unsigned int block_area = n_in_bytes * 8;
  if(height < 2 || width < 4){
    fprintf(stderr, "image needs to be at least 2 pixels (height) by 4 pixels (width)\n");
    free(in_bytes);
    lodepng_state_cleanup(&state);
    free(decoded_image_data);
    return 1;
  }

  if(block_area > (width * (height - 1))){
    fprintf(stderr, "not enough space in image to encode data\nmax number_of_bytes to encrypt = %d\n", (width * (height - 1)) / 8);
    free(in_bytes);
    lodepng_state_cleanup(&state);
    free(decoded_image_data);
    return 1;
  }

  unsigned int height_mask = 1;
  while(height_mask < height){
    height_mask = (height_mask << 1) + 1;
  }

  unsigned int width_mask = 1;
  while(width_mask < width){
    width_mask = (width_mask << 1) + 1;
  }

  int index = 0;
  count = 0;
  while((block_width * block_height) < block_area || block_width > width || block_height > (height - 1)){
    if(count < 1000){
      if(index + 1 > 255){
        isaac(isaac_buffer, 256);
        index = 0;
      }

      block_height = isaac_buffer[index] & height_mask;
      block_width = isaac_buffer[index + 1] & width_mask;
      index += 2;
    }else{
      if(block_height > (height - 1)) block_height = height - 2;
      if(block_width > width) block_width = width - 1;
      if((block_width * block_height) < block_area){
        block_width = width;
        block_height = height - 1;
      }

      break;
    }
    count++;
  }

  count = 0;
  do{
    if(count < 1000){
      if(index + 1> 255){
        isaac(isaac_buffer, 256);
        index = 0;
      }

      block_x = isaac_buffer[index] & width_mask;
      block_y = isaac_buffer[index + 1] & height_mask;
      index += 2;
    }else{
      if(block_y + block_height > height) block_y = 1;
      if(block_x + block_width > width) block_x = 0;
      if(block_y < 1) block_y = 1;
      break;
    }
    
    count++; 
  }while((block_x + block_width > width) || (block_y + block_height > (height - 1)) || block_y < 1);

  int bitdepth = state.info_raw.bitdepth;
  int bits_per_pixel = -1;
  switch(state.info_raw.colortype){
    case LCT_GREY:
      if(bitdepth == 1 || bitdepth == 2 || bitdepth == 4 || bitdepth == 8 || bitdepth == 16){
        bits_per_pixel = bitdepth;
      }
      break;
    case LCT_RGB:
      if(bitdepth == 8 || bitdepth == 16){
        bits_per_pixel = 3 * bitdepth;
      }
      break;
    case LCT_PALETTE:
      if(bitdepth == 1 || bitdepth == 2 || bitdepth == 4 || bitdepth == 8){
        bits_per_pixel = bitdepth;
      }
      break;
    case LCT_GREY_ALPHA:
      if(bitdepth == 8 || bitdepth == 16){
        bits_per_pixel = bitdepth * 2;
      }
      break;
    case LCT_RGBA:
      if(bitdepth == 8 || bitdepth == 16){
        bits_per_pixel = 4 * bitdepth;
      }
      break;
    default:
      break;
  }

  if(bits_per_pixel == -1){
    fprintf(stderr, "invalid bitdepth and color type\n");
    free(in_bytes);
    lodepng_state_cleanup(&state);
    free(decoded_image_data);
    return 1;
  }

  if(bits_per_pixel % 8 == 0){
    int bytes_per_pixel = bits_per_pixel / 8;
    for(int x = 0; x < (sizeof(uint32_t) * 8); x++){
      if((1 & (n_in_bytes >> ((sizeof(uint32_t) * 8) - 1 - x))) ^ (1 & first_isaac_buffer[x])){
        decoded_image_data[bytes_per_pixel * x] |= (1);
      }else{
        decoded_image_data[bytes_per_pixel * x] &= (~1);
      }
    }

    index = 0;
    int rand_count = 0;
    int pixel_index = 0;
    for(int x = 0; x < block_width; x++){
      if(index > n_in_bytes * 8 - 1) break;
      for(int y = 0; y < block_height; y++){
        if(index > n_in_bytes * 8 - 1) break;
    
        if(rand_count > 255){
          rand_count = 0;
          isaac(isaac_buffer, 256);
        }

        if(bytes_per_pixel > 1) pixel_index = isaac_buffer[rand_count] % (bytes_per_pixel - 1);

        if((1 & isaac_buffer[rand_count]) ^ (1 & (in_bytes[index / 8] >> (7 - (index % 8))))){
          decoded_image_data[bytes_per_pixel * (y + block_y) * width + bytes_per_pixel * (x + block_x) + pixel_index] |= 1;
        }else{
          decoded_image_data[bytes_per_pixel * (y + block_y) * width + bytes_per_pixel * (x + block_x) + pixel_index] &= (~1);
        }

        rand_count++;
        index++;
      }
    }
  }else{
    fprintf(stderr, "error\n");
    free(in_bytes);
    lodepng_state_cleanup(&state);
    free(decoded_image_data);
    return 1;
  }
  free(in_bytes);

  unsigned char* output_png = 0;
  int error;
  error = lodepng_encode(&output_png, &png_size, decoded_image_data, width, height, &state);

  free(decoded_image_data);
  lodepng_state_cleanup(&state);

  if(error){
    fprintf(stderr, "couldn't encode image data\n");
    return 1;
  }

  error = lodepng_save_file(output_png, png_size, file_name);

  free(output_png);
  if(error){
    fprintf(stderr, "couldn't save to file\n");
    return 1;
  }

  return 0;
}