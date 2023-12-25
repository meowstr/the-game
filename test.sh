docker run \
  --rm \
  -v $(pwd):/src \
  -u $(id -u):$(id -g) \
  -it \
  emscripten/emsdk \
  bash
