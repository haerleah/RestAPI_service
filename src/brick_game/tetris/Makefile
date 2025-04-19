CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra -pedantic -lpthread
LIBFLAGS = -shared -fPIC
OUTPUT = libs21_tetris.so

SRC_FILES = s21_tetris_back.c \
            s21_controller.c

all: compile_library

compile_library: $(SRC_FILES)
	$(CC) $(CFLAGS) $(LIBFLAGS) $(SRC_FILES) -o $(OUTPUT)

clean:
	rm -rf $(OUTPUT)
