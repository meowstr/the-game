#pragma once

struct res_t {
    unsigned char * data;
    int size;
};

namespace res {

res_t shaders_glsl();
res_t miku_jpg();

}

