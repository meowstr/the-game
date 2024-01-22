#include "hardware.hpp"

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include <emscripten.h>
#include <emscripten/html5.h>

#include "logging.hpp"

static struct {
    GLFWwindow * window = nullptr;
    int width = 800;
    int height = 600;
} hardware;

int hardware_init()
{
    if ( !glfwInit() ) {
        ERROR_LOG( "failed to initialize glfw" );
        return 1;
    }

    glfwWindowHint( GLFW_RESIZABLE, 1 );
    hardware.window = glfwCreateWindow(
        hardware.width,
        hardware.height,
        "meow",
        nullptr,
        nullptr
    );

    if ( !hardware.window ) {
        ERROR_LOG( "failed to create window" );
        return 1;
    }

    glfwMakeContextCurrent( hardware.window );

    // glfwSetWindowSizeCallback( hardware.window, handle_resize );

    {
        double width, height;
        emscripten_get_element_css_size( "canvas", &width, &height );
        emscripten_set_canvas_element_size(
            "canvas",
            int( width ),
            int( height )
        );
        // emscripten_set_resize_callback(
        //     "canvas",
        //     nullptr,
        //     false,
        //     on_web_display_size_changed
        //);

        hardware.width = (int) width;
        hardware.height = (int) height;
    }

    glfwSetWindowSize( hardware.window, hardware.width, hardware.height );
    glViewport( 0, 0, hardware.width, hardware.height );

    return 0;
}

void hardware_destroy()
{
    glfwDestroyWindow( hardware.window );
    glfwTerminate();
}

using loop_function_t = void ( * )();

void hardware_set_loop( loop_function_t step )
{
    emscripten_set_main_loop( step, 0, 1 );
}

int hardware_width()
{
    return hardware.width;
}

int hardware_height()
{
    return hardware.height;
}

float hardware_x_axis()
{
    float dx = 0.0f;

    if ( glfwGetKey( hardware.window, GLFW_KEY_A ) == GLFW_PRESS ) {
        dx -= 1.0f;
    }

    if ( glfwGetKey( hardware.window, GLFW_KEY_D ) == GLFW_PRESS ) {
        dx += 1.0f;
    }

    if ( glfwGetMouseButton( hardware.window, GLFW_MOUSE_BUTTON_LEFT ) ==
         GLFW_PRESS ) {
        double x, y;
        glfwGetCursorPos( hardware.window, &x, &y );
        if ( x < hardware.width * 0.5f ) {
            dx = -1.0f;
        } else {
            dx = 1.0f;
        }
    }

    return dx;
}

float hardware_time()
{
    return glfwGetTime();
}

void hardware_rumble()
{
}
