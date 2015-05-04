CC = gcc
FILES = src/*.c src/methods/*.c
OUT_EXE = rb-stegano

build: $(FILES)
	$(CC) -o $(OUT_EXE) $(FILES)