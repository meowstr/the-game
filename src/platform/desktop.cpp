#include "hardware.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "logging.hpp"

static struct {
    GLFWwindow * window = nullptr;
    int width = 800;
    int height = 600;
} intern;

int hardware_init()
{
    if ( !glfwInit() ) {
        ERROR_LOG( "failed to initialize glfw" );
        return 1;
    }

    // glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
    // glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
    // glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
    // glfwWindowHint( GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API );

    glfwWindowHintString( GLFW_X11_CLASS_NAME, "floater" );
    glfwWindowHintString( GLFW_X11_INSTANCE_NAME, "floater" );
    glfwWindowHint( GLFW_RESIZABLE, 1 );

    intern.window = glfwCreateWindow(
        intern.width,
        intern.height,
        "meow",
        nullptr,
        nullptr
    );

    if ( !intern.window ) {
        ERROR_LOG( "failed to create window" );
        return 1;
    }

    glfwMakeContextCurrent( intern.window );

    if ( !gladLoadGLES2Loader( (GLADloadproc) glfwGetProcAddress ) ) {
        ERROR_LOG( "failed to load OpenGL" );
        return 1;
    }

    const char * gl_version = (const char *) glGetString( GL_VERSION );
    INFO_LOG( "OpenGL version: %s", gl_version );

    glfwSwapInterval( 1 );
    glViewport( 0, 0, intern.width, intern.height );

    return 0;
}

void hardware_destroy()
{
    glfwDestroyWindow( intern.window );
    glfwTerminate();
}

using loop_function_t = void ( * )();

void hardware_set_loop( loop_function_t step )
{
    while ( !glfwWindowShouldClose( intern.window ) ) {
        glfwPollEvents();
        step();
        glfwSwapBuffers( intern.window );
    }
}

int hardware_width()
{
    return intern.width;
}

int hardware_height()
{
    return intern.height;
}

float hardware_x_axis()
{
    float dx = 0.0f;

    if ( glfwGetKey( intern.window, GLFW_KEY_A ) == GLFW_PRESS ) {
        dx -= 1.0f;
    }

    if ( glfwGetKey( intern.window, GLFW_KEY_D ) == GLFW_PRESS ) {
        dx += 1.0f;
    }

    if ( glfwGetMouseButton( intern.window, GLFW_MOUSE_BUTTON_LEFT ) ==
         GLFW_PRESS ) {
        double x, y;
        glfwGetCursorPos( intern.window, &x, &y );
        if ( x < intern.width * 0.5f ) {
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
