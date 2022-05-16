all:
	clang -O3 -lm -lSDL src/*.c -o viz

debug:
	clang -g -Og -lm -lSDL src/*.c -o dviz
