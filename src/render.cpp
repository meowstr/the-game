#include "render.hpp"

#include <GLES2/gl2.h>

struct {
} rstate;



void render_init()
{
}

void render()
{
    glClearColor( 0.1f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
}
