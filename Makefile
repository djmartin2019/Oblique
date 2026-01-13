# Compiler & flags
CC = gcc
CFLAGS = -Wall -std=c99 -Iinclude -Iengine
SDL_CFLAGS = `sdl2-config --cflags`
SDL_LIBS = `sdl2-config --libs` -lSDL2_image

# Project structure
SRC = \
    src/main.c \
    engine/core/map.c \
    engine/core/scene.c \
    engine/core/input.c \
    engine/core/constants.c \
    engine/render/camera.c \
    engine/render/render.c \
    engine/helpers/sdl_helpers.c \
    engine/entity/entity.c \
    engine/entity/player.c \
    engine/ai/behavior.c \
    engine/ui/ui.c \
	engine/navigation/grid.c

BIN = oblique

# Build rule
all:
	$(CC) $(SRC) -o $(BIN) $(CFLAGS) $(SDL_CFLAGS) $(SDL_LIBS)

# Clean rule
clean:
	rm -f $(BIN)

