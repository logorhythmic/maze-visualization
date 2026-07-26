SRCS = \
	state_manager.c \
	maze.c \
	maze_events.c \
	maze_render.c \
	generation.c \
	solving.c 

SRC_FILES = main.c $(addprefix src/, $(SRCS))

run: 
	gcc $(SRC_FILES) -Iinclude -o output.bin -g -Og -lSDL3 && ./output.bin

san:
	gcc $(SRC_FILES) -fsanitize=address -Iinclude -g -O0 -lSDL3 -o output.bin 
