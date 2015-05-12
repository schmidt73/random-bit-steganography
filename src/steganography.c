#include "steganography.h"

#define SUPPORTED_FILE_TYPES {"png"}
#define PROGRAM_VERSION "0.5"
#define PROGRAM_NAME "random-bit stegano"

const char* get_filename_ext(const char *filename) 
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

void print_version()
{
	fprintf(stderr, "%s is a program created by Henri Schmidt.\nCurrently running version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
}

void print_usage()
{
	const char* supported_files[] = SUPPORTED_FILE_TYPES;

	fprintf(stderr, "usage: %s [-t filetype] -m encryption | decryption -k encryptionkey -i inputfile [< | > datastream]\n", PROGRAM_NAME);
	fprintf(stderr, "\t-m mode: encryption or decryption\n");
	fprintf(stderr, "\t-k encryptionkey: up to a 1024-byte encryption key\n\t\texample: -k 58313907894542357986579124\n");
	fprintf(stderr, "\t-t filetype: the type of file of the inputfile\n");
	fprintf(stderr, "\t\tcurrently supported file types:");

	for(int i = 0; i < sizeof(supported_files) / sizeof(supported_files[0]); i++){
		fprintf(stderr, " %s", supported_files[i]);
	}

	fprintf(stderr, " (attempts to detect filetype based off extension if not specified)\n");
	fprintf(stderr, "\t-i inputfile: the file the data will be hidden inside of if encyp\n");
	fprintf(stderr, "\tdatastream\n\t\tduring encryption a pipe containing the data to encrypt\n\t\tduring decryption a pipe to the output of the decryption\n");
	fprintf(stderr, "\ntypical usage example:\n");
	fprintf(stderr, "\tencrypt data from inputdata.txt in png test.png: %s -m encryption -k 12321312312312312312312 -i test.png < inputdata.txt\n", PROGRAM_NAME);
	fprintf(stderr, "\tdecrypt data from test.png and put output in output.txt: %s -m decryption -k 12321312312312312312312 -i test.png > output.txt\n", PROGRAM_NAME);
}

int init_decrypting_data(const char* type, const char* inputfile)
{
	int ret = 0;

	if(strcmp(type, "gif") == 0){

	}else if(strcmp(type, "jpeg") == 0 || strcmp(type, "jpg") == 0){

	}else if(strcmp(type, "mp3") == 0){

	}else if(strcmp(type, "png") == 0){
		if((ret = init_png_file(inputfile)) == 0){
			if((ret = decrypt_png_file()) == 0){
				fprintf(stderr, "sucessfully decrypted data from %s\n", inputfile);
			}
		}
	}else{
		fprintf(stderr, "undetected file type: %s\nplease specify a known file type\n", type);
		ret = 1;
	}

	return ret;
}

int init_encrypting_data(const char* type, const char* inputfile)
{
	int ret = 0;

	if(strcmp(type, "gif") == 0){

	}else if(strcmp(type, "jpeg") == 0 || strcmp(type, "jpg") == 0){

	}else if(strcmp(type, "mp3") == 0){

	}else if(strcmp(type, "png") == 0){
		if((ret = init_png_file(inputfile)) == 0){
			if((ret = encrypt_png_file()) == 0){
				fprintf(stderr, "sucessfully encrypted data into %s\n", inputfile);
			}
		}
	}else{
		fprintf(stderr, "undetected file type: %s\nplease specify a known file type\n", type);
		ret = -1;
	}

	return ret;
}

int main(int argc, char **argv)
{	
	const char* type = 0, *inputfile = 0, *mode = 0;
	char key[1024] = {0};
	unsigned int key_length = 0;
	int c = -1;

	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-help") == 0){
			print_usage();
			return -1;
		}else if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-version") == 0){
			print_version();
			return -1;
		}
	}

	while((c = getopt(argc, argv, "m:t:i:k:hv")) != -1){
		switch(c){
			case 'k':
				if(strlen(optarg) < 1024){
					memcpy(key, optarg, strlen(optarg));
				}else{
					memcpy(key, optarg, 1024);
				}
				break;
			case 'm':
				mode = optarg;
				break;
			case 't':
				type = optarg;
				break;
			case 'i':
				inputfile = optarg;
				break;
			case 'v':
				print_version();
				return -1;
				break;
			case 'h':
				print_usage();
				return -1;
				break;
			case '?':
				print_usage();
				return -1;
				break;
			default:
				fprintf(stderr, "Error!");
		}
	}

	if(inputfile == 0 || key == 0 || mode == 0){
		print_usage();
		return -1;
	}

	if(type == 0){
		type = get_filename_ext(inputfile);
	}

	set_key(key, 1024);

	for(int i = 0; i < 10; i++){
    	isaac(0, 0);
  	}

	if(strcmp(mode, "encryption") == 0){
		if(isatty(STDIN_FILENO)){
			print_usage();
			return -1;
		}

		return init_encrypting_data(type, inputfile);
	}
	else if(strcmp(mode, "decryption") == 0){
		if(isatty(STDOUT_FILENO)){
			print_usage();
			return -1;
		}

		return init_decrypting_data(type, inputfile);
	}else{
		print_usage();
		return -1;
	}
}