SOURCES = src/*.cpp
HEADERS = src/*.hpp
FLAGS = -std=c++20 -g -Ibuild -Isrc
RES_FILE = build/res.h


web: bake
	mkdir -p build
	emcc       \
		--shell-file shell.html \
		-sMIN_WEBGL_VERSION=2   \
		-sMAX_WEBGL_VERSION=2   \
		-sUSE_GLFW=3            \
		${FLAGS}                \
		${SOURCES}              \
		src/platform/web.cpp    \
		libs/web/*.a            \
		-o build/index.html

linux: bake
	mkdir -p build
	g++ ${FLAGS}                 \
		${SOURCES}               \
		src/platform/desktop.cpp \
		src/platform/glad.c      \
		-lglfw                   \
		-o build/game

windows: bake
	mkdir -p build
	x86_64-w64-mingw32-g++       \
		-Isrc/platform           \
		${FLAGS}                 \
		${SOURCES}               \
		src/platform/desktop.cpp \
		src/platform/glad.c      \
		libs/win32/*.a           \
		-lgdi32                  \
		-lssp                    \
		-static                  \
		-o build/game

docker-emscripten:
	docker run                           \
		--rm                             \
		-v $(shell pwd):/src             \
		-u $(shell id -u):$(shell id -g) \
		emscripten/emsdk                 \
		make web


bake:
	mkdir -p build
	rm -rf ${RES_FILE}
	gcc tools/bake.c -o build/bake
	./build/bake src/shaders.glsl >> ${RES_FILE}

clean:
	rm -rf build
