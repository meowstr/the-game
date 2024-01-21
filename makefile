SOURCES = src/*.cpp
HEADERS = src/*.hpp
LIBS = libs/*.a
FLAGS = -std=c++20 -g -Ibuild
RES_FILE = build/res.h


build/main.html: ${SOURCES} ${HEADERS} bake
	mkdir -p build
	emcc       \
		--shell-file shell.html \
		-sMIN_WEBGL_VERSION=2   \
		-sMAX_WEBGL_VERSION=2   \
		-sUSE_GLFW=3            \
		${FLAGS}                \
		${SOURCES}              \
		${LIBS}                 \
		-o build/index.html

bake:
	mkdir -p build
	rm -rf ${RES_FILE}

	gcc tools/bake.c -o build/bake
	./build/bake src/shaders.glsl >> ${RES_FILE}

bench:
	mkdir -p build
	for source in ${SOURCES}; do \
	  echo $${source};           \
	  emcc                       \
	    -c                       \
		${FLAGS}                 \
	    $${source}               \
	    -o build/tmp.o;          \
	done

clean:
	rm -rf build
