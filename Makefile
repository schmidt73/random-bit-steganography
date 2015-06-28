# Declaration of variables
CC = gcc
CC_FLAGS = -Wall -Wextra -O2

# File names
EXEC = rb-stegano
SOURCES = $(wildcard src/*.c deps/lodepng/*.c)
OBJECTS = $(SOURCES:.c=.o)

# Dependencies MUST COMPILE mp3lame with HAVE_MPGLIB
LIBRARIES = mp3lame mpg123
LIBRARY_PARAMS = $(foreach d, $(LIBRARIES), -l$d)
INCLUDE = deps

# Includes
INC = $(INCLUDE) $(INC_DIR2)
INC_PARAMS = $(foreach d, $(INC), -I$d)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(LIBRARY_PARAMS) $(OBJECTS) -o $(EXEC)

# To obtain object files
%.o: %.c
	$(CC) $(INC_PARAMS) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)