# Compiler and linker
CC = g++
CFLAGS = -Wall -g `sdl2-config --cflags` -I/usr/include/SDL2
LDFLAGS = `sdl2-config --libs` -lSDL2main -lSDL2_image -lSDL2_mixer -lSDL2_ttf

# Source files
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# Executable
EXEC = sdl_app

# Rules
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC)

.PHONY: all clean run