#include "hardware.hpp"
#include "logging.hpp"
#include "render.hpp"
#include "state.hpp"

#include <GLES2/gl2.h>

#include <math.h>

static void loop()
{
    static float time = 0.0f;
    time += 1.0f / 60.0f;

    render();
}

int main()
{
    INFO_LOG( "meow" );

    hardware_init();

    render_init();

    hardware_set_loop( loop );

    return 0;
}
