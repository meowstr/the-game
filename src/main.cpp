#include "hardware.hpp"
#include "logging.hpp"

#include <GLES2/gl2.h>

#include <math.h>

static void loop()
{
    static float time = 0.0f;

    time += 1.0f / 60.0f;

    glClearColor( 0.05f + 0.05f * sin( time ), 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
}

int main()
{
    INFO_LOG( "meow" );

    hardware_init();

    hardware_set_loop( loop );

    return 0;
}
