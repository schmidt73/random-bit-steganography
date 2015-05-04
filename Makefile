CC = gcc
FILES = src/*.c src/methods/*.c
OUT_EXE = out_executable

build: $(FILES)
	$(CC) -o $(OUT_EXE) $(FILES)