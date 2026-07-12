make run:
	gcc main.c src/*.c -Iinclude -o output.bin -g -Og -lSDL3 && ./output.bin

make san:
	gcc main.c src/*.c -fsanitize=address -Iinclude -g -Og -lSDL3 -o output.bin && ./output.bin
