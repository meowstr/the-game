SOURCES = src/*.cpp
HEADERS = src/*.h
FLAGS = -std=c++98

build/main.html: ${SOURCES} ${HEADERS}
	mkdir -p build
	emcc       \
		--shell-file shell.html \
		-sMIN_WEBGL_VERSION=2   \
		-sMAX_WEBGL_VERSION=2   \
		-sUSE_SDL		\
		${FLAGS}                \
		${SOURCES}              \
		-o build/index.html


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
