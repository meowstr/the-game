#include "res.hpp"
#include "res_data.h"

res_t res::shaders_glsl()
{
    return { ::src_shaders_glsl, sizeof( ::src_shaders_glsl ) };
}

res_t res::miku_jpg()
{
    return { ::res_miku_jpg, sizeof( ::res_miku_jpg ) };
}
