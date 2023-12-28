package_version="0.9.1"
package_name="cglm-${package_version}"
package_upstream="https://github.com/recp/cglm/archive/refs/tags/v${package_version}.tar.gz"


curl -LJO ${package_upstream}
tar -xzvf "${package_name}.tar.gz"

docker run --rm -v "$(pwd)/${package_name}:/src" \
  -u $(id -u):$(id -g) \
  -it emscripten/emsdk \
  sh -c "emcmake cmake -S . -B build                     \
           -DCMAKE_EXE_LINKER_FLAGS='-s STANDALONE_WASM' \
           -DCGLM_STATIC=ON &&                           \
         cmake --build build"
