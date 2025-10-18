CC=emcc
SOURCES:=$(wildcard *.c) 
LDFLAGS=-O2
OUTPUT=opengl.html


all: $(SOURCES) 
	gcc TestEmscripten.c -lm -lSDL2_image -lSDL2 -lGL -lGLEW -o isometric
debug: $(SOURCES) 
	gcc TestEmscripten.c -g -lm -lSDL2_image -lSDL2 -lGL -lGLEW -o isometric

webgl: $(SOURCES) *.h
	$(CC) $(SOURCES) -g --bind -s FULL_ES3=1 -s USE_SDL=2 -s WASM=1 -s USE_SDL_IMAGE=2 -sASYNCIFY -std=c11 -I $(LDFLAGS) -o $(OUTPUT)

clean:
	rm $(OUTPUT)
