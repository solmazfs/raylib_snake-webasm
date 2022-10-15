CC = emcc
PLATFORM=PLATFORM_WEB
INCLUDE_PATHS=../raylib/src

build:
	mkdir build
	$(CC) -o build/index.html main.c timer.c -Os -Wall -I $(INCLUDE_PATHS) -L $(INCLUDE_PATHS) -s USE_GLFW=3 -s ASYNCIFY --shell-file minshell.html -D$(PLATFORM) -lraylib

clean:
	rm -rf build/

run:
	cd build/ && python -m http.server
