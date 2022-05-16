all:
	clang -O3 -lm -lSDL2 src/*.c -o viz

debug:
	clang -g -Og -lm -lSDL2 src/*.c -o dviz
