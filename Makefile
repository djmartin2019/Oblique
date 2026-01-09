CC = gcc
CFLAGS = -Wall -std=c99 -Iinclude
SDL_CFLAGS = `sdl2-config --cflags`
SDL_LIBS = `sdl2-config --libs` -lSDL2_image

SRC = src/main.c src/map.c src/render.c src/player.c src/camera.c src/entity.c src/behavior.c
BIN = oblique

all:
	$(CC) $(SRC) -o $(BIN) $(CFLAGS) $(SDL_CFLAGS) $(SDL_LIBS)

clean:
	rm -f $(BIN)
