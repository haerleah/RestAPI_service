CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic
EXTRA_FLAGS = -lcurl -ljson-c
CURSES_FLAGS = -lcurses -lmenu

SRC_FILES = s21_cli.c s21_cli_main.c
NETWORK_LIBRARY = s21_client_library.a
OUTPUT = s21_cli

all: cli

cli: $(NETWORK_LIBRARY) $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) $(NETWORK_LIBRARY) -o $(OUTPUT) $(EXTRA_FLAGS) $(CURSES_FLAGS)

$(NETWORK_LIBRARY):
	cd ../../client_library && make
	cp ../../client_library/$(NETWORK_LIBRARY) .