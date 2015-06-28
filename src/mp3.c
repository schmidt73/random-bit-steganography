#include "mp3.h"

static const char* file = NULL;
static mpg123_handle* handle = NULL;
static lame_global_flags* lame_flags = NULL;
static int channels = 0, encoding = 0;
static long rate = 0;

static void cleanup_mpghandle(){
	if(handle != NULL){
		mpg123_close(handle);
		mpg123_delete(handle);
	}

	mpg123_exit();
}

int init_mp3_file(const char* inputfile){
	file = inputfile;

	int error = mpg123_init();

	if(error != MPG123_OK || (handle = mpg123_new(NULL, &error)) == NULL){
		print_error(stderr, "failed to initialize mpg123\n");
		cleanup_mpghandle();
		return 1;
	}

	error = mpg123_format_all(handle);
	if(error != MPG123_OK){
		print_error(stderr, "couldn't set video format\n");
		cleanup_mpghandle();
		return 1;
	}

	error = mpg123_open(handle, inputfile);
	if(error != MPG123_OK){
		print_error(stderr, "couldn't open mp3 file\n");
		cleanup_mpghandle();
		return 1;
	}

	error = mpg123_getformat(handle, &rate, &channels, &encoding);
	if(error != MPG123_OK){
		print_error(stderr, "couldn't read file format\n");
		cleanup_mpghandle();
		return 1;
	}

	print_status(stderr, "mp3 format: %li Hz, %i channels, encoding value %i\n", rate, channels, encoding);

	lame_flags = lame_init();
	if(lame_flags == NULL){
		print_error(stderr, "couldn't initilize LAME\n");
		cleanup_mpghandle();
		return 1;
	}

	lame_set_num_channels(lame_flags, channels);
	lame_set_in_samplerate(lame_flags, rate);
	error = lame_init_params(lame_flags);
	if(error < 0){
		print_error(stderr, "couldn't set LAME params.\n");
		cleanup_mpghandle();
		return 1;
	}

	return 0;
}

int decrypt_mp3_file(){
	off_t offset = 0;
	size_t audio_size = 0;
	unsigned char* audio = NULL;

	int error;
	do{
		error = mpg123_decode_frame(handle, &offset, &audio, &audio_size);

		if(error == MPG123_NEW_FORMAT){
			error = mpg123_getformat(handle, &rate, &channels, &encoding);
			print_status(stderr, "here\n");
			if(error != MPG123_OK){
				print_error(stderr, "couldn't read mp3 chunk format\n");
				break;
			}
		}else if(error != MPG123_OK){
			break;
		}

		fwrite(audio, 1, audio_size, stdout);
		//unsigned char buffer[audio_size];
		//lame_encode_buffer_interleaved(lame_flags, audio, audio_size, buffer, audio_size);
		//fwrite(buffer, sizeof(char), audio_size, stdout);
	}while(error == MPG123_OK && audio_size);

	cleanup_mpghandle();

	if(error == MPG123_DONE){
		print_success(stderr, "mp3 successfully decrypted! offset %d\n", offset);
		return 1;
	}else{
		print_error(stderr, "unable to decrypt mp3\n");
		return 0;
	}
}

int encrypt_mp3_file(){
	return 0;
}