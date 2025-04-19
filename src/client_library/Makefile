CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra -pedantic

OUTPUT = s21_client_library.a

SRC_FILES = s21_client_library.c

OBJ_FILES = $(SRC_FILES:.c=.o)

all: collect_library

collect_library: $(OBJ_FILES)
	ar rc $(OUTPUT) $(OBJ_FILES)
	ranlib $(OUTPUT)
	make clean

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_FILES)