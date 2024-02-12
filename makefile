SOURCES = src/*.cpp
HEADERS = src/*.hpp
FLAGS = -std=c++20 -g -Ibuild -Isrc
RES_FILE = build/res_data.h


web:
	mkdir -p build
	emcc                      \
	  --shell-file shell.html \
	  -sMIN_WEBGL_VERSION=2   \
	  -sMAX_WEBGL_VERSION=2   \
	  -sUSE_GLFW=3            \
	  -sALLOW_MEMORY_GROWTH   \
	  -sTOTAL_MEMORY=33554432 \
	  ${FLAGS}                \
	  ${SOURCES}              \
	  src/platform/web.cpp    \
	  libs/web/*.a            \
	  libs/web/res.o          \
	  -lopenal                \
	  -o build/index.html

linux-sdl:
	mkdir -p build
	g++ ${FLAGS}                   \
	  ${SOURCES}                   \
	  src/platform/desktop-sdl.cpp \
	  src/platform/glad.c          \
	  libs/linux/res.o             \
	  -lSDL2                       \
	  -lopenal                     \
	  -o build/meow

linux:
	mkdir -p build
	g++ ${FLAGS}               \
	  ${SOURCES}               \
	  src/platform/desktop.cpp \
	  src/platform/glad.c      \
	  libs/linux/res.o         \
	  -lglfw                   \
	  -lopenal                 \
	  -o build/meow

windows:
	mkdir -p build
	x86_64-w64-mingw32-g++     \
	  -Ilibs/win32             \
	  -DAL_LIBTYPE_STATIC      \
	  ${FLAGS}                 \
	  ${SOURCES}               \
	  src/platform/desktop.cpp \
	  src/platform/glad.c      \
	  libs/win32/*.a           \
	  libs/win32/*.lib         \
	  libs/win32/res.o         \
	  -lgdi32                  \
	  -lssp                    \
	  -lwinmm                  \
	  -lole32                  \
	  -static                  \
	  -o build/meow

docker-emscripten:
	docker run                         \
	  --rm                             \
	  -v $(shell pwd):/src             \
	  -u $(shell id -u):$(shell id -g) \
	  emscripten/emsdk                 \
	  make web

docker-steam:
	docker run                                         \
	  --rm                                             \
	  --init                                           \
	  -v $(shell pwd):/home                            \
	  -u $(shell id -u):$(shell id -g)                 \
	  -w /home                                         \
	  registry.gitlab.steamos.cloud/steamrt/sniper/sdk \
	  make linux-sdl

docker-android:
	docker run                               \
	  --rm                                   \
	  -v $(shell pwd):/home/circleci/project \
	  -it cimg/android:2024.01-ndk           \
	  bash -i

bake:
	mkdir -p build
	gcc -std=c99 tools/bake.c -o build/bake
	./build/bake        \
	  src/shaders.glsl  \
	  res/*.png         \
	  res/*.fnt         \
	  res/*.wav         \
	  res/*.jpg         \
	  > ${RES_FILE}

bake-web: bake
	emcc -c ${FLAGS} tools/res.cpp -o libs/web/res.o

bake-windows: bake
	x86_64-w64-mingw32-g++ -c ${FLAGS} tools/res.cpp -o libs/win32/res.o

bake-linux: bake
	g++ -c ${FLAGS} tools/res.cpp -o libs/linux/res.o

clean:
	rm -rf build
