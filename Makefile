CC = gcc
CFLAGS = -Wall -std=c99 -Iinclude
LDFLAGS = `sdl2-config --cflags --libs`

SRC = src/main.c
BIN = oblique

all:
	$(CC) $(SRC) -o $(BIN) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(BIN)
