# ─────────────────────────────────────────────────────────────────────
# Makefile — invaders-rl (macOS, raylib 6.0 built from source)
#
# Usage:
#   make          — build the game
#   make run      — build and run
#   make clean    — remove build output
# ─────────────────────────────────────────────────────────────────────

RAYLIB_SRC = build/external/raylib/src
RAYLIB_LIB = $(RAYLIB_SRC)/libraylib.a

SRC    = src/*.c
TARGET = bin/game

CC     = clang
CFLAGS = -std=c99 -g -Wall -I$(RAYLIB_SRC) -Iinclude
FRAMEWORKS = -framework CoreVideo -framework IOKit \
             -framework Cocoa -framework GLUT -framework OpenGL

.PHONY: all run clean raylib

all: $(TARGET)

$(TARGET): $(SRC) $(RAYLIB_LIB) | bin
	$(CC) $(CFLAGS) $(SRC) $(RAYLIB_LIB) $(FRAMEWORKS) -o $(TARGET)

run: all
	./$(TARGET)

# Build raylib 6.0 from source (only needed once)
raylib:
	$(MAKE) -C $(RAYLIB_SRC) PLATFORM=PLATFORM_DESKTOP

bin:
	mkdir -p bin

clean:
	rm -f $(TARGET)
