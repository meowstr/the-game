#include "hardware.hpp"

#include <SDL2/SDL.h>

#include <glad/glad.h>

#include "logging.hpp"

static struct {
    SDL_Window * window = nullptr;
    SDL_GLContext context = nullptr;
    SDL_Joystick * joy1 = nullptr;
    int width = 1280;
    int height = 800;
} intern;

int hardware_init()
{
    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ) {
        ERROR_LOG( "failed to initialize SDL2" );
        return 1;
    }

    SDL_JoystickEventState( SDL_ENABLE );

    INFO_LOG( "found %d joystick(s)", SDL_NumJoysticks() );

    intern.joy1 = SDL_JoystickOpen( 0 );

    SDL_GL_LoadLibrary( nullptr );

    // glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
    // glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
    // glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
    // glfwWindowHint( GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API );

    intern.window = SDL_CreateWindow(
        "meow",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        intern.width,
        intern.height,
        SDL_WINDOW_OPENGL
    );

    if ( !intern.window ) {
        ERROR_LOG( "failed to create window" );
        return 1;
    }

    intern.context = SDL_GL_CreateContext( intern.window );

    if ( !intern.context ) {
        ERROR_LOG( "failed to create OpenGL context" );
        return 1;
    }

    if ( !gladLoadGLES2Loader( (GLADloadproc) SDL_GL_GetProcAddress ) ) {
        ERROR_LOG( "failed to load OpenGL" );
        return 1;
    }

    const char * gl_version = (const char *) glGetString( GL_VERSION );
    INFO_LOG( "OpenGL version: %s", gl_version );

    SDL_GL_SetSwapInterval( 1 );
    glViewport( 0, 0, intern.width, intern.height );

    return 0;
}

void hardware_destroy()
{
    SDL_DestroyWindow( intern.window );
    SDL_JoystickClose( intern.joy1 );
    SDL_Quit();
}

using loop_function_t = void ( * )();

void hardware_set_loop( loop_function_t step )
{
    int quit_loop = 0;
    while ( !quit_loop ) {
        SDL_Event event;
        while ( SDL_PollEvent( &event ) ) {
            if ( event.type == SDL_QUIT ) quit_loop = 1;
        }
        step();
        SDL_GL_SwapWindow( intern.window );
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

    const Uint8 * keys = SDL_GetKeyboardState( nullptr );

    if ( keys[ SDL_SCANCODE_A ] ) {
        dx -= 1.0f;
    }

    if ( keys[ SDL_SCANCODE_D ] ) {
        dx += 1.0f;
    }

    if ( intern.joy1 ) {
        Sint16 joy_dx = SDL_JoystickGetAxis( intern.joy1, 0 );

        if ( joy_dx > 10000 ) {
            dx += 1.0f;
        }
        if ( joy_dx < -10000 ) {
            dx -= 1.0f;
        }
    }

    return dx;
}

float hardware_time()
{
    return SDL_GetTicks() / 1000.0f;
}

void hardware_rumble()
{
    if ( intern.joy1 ) {
        SDL_JoystickRumble( intern.joy1, 0x8000, 0x8000, 100 );
    }
}
