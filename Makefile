CC = gcc
FILES = src/*.c src/methods/*.c src/libs/*.c
OUT_EXE = rb-stegano

build: $(FILES)
	$(CC) -o $(OUT_EXE) $(FILES)