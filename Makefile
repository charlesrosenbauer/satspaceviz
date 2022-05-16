all:
	clang -O3 -lm -lpthread -lSDL2 src/*.c -o viz

debug:
	clang -g -Og -lpthread -lm -lSDL2 src/*.c -o dviz
