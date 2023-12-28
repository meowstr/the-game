#include "render.hpp"

#include "render_utils.hpp"
#include "state.hpp"

#include "cglm/affine.h"
#include "cglm/mat4.h"

#include <GLES2/gl2.h>

// render state
struct {
    pos_buffer_t triangle_buffer;

    struct {
        int id;
        int proj;
        int color;
    } shader;

    mat4 proj;

} rstate;

static void shader_init()
{
    const char * v_shader_str = "#version 100                        \n"
                                "attribute vec3 a_pos;               \n"
                                "uniform mat4 u_proj;                \n"
                                "void main()                         \n"
                                "{                                   \n"
                                "   vec4 pos = vec4(a_pos, 1.0);     \n"
                                "   gl_Position = u_proj * pos;      \n"
                                "}                                   \n";

    const char * f_shader_str = "#version 100                        \n"
                                "precision lowp float;               \n"
                                "uniform vec4 u_color;               \n"
                                "void main()                         \n"
                                "{                                   \n"
                                "  gl_FragColor = u_color;           \n"
                                "}                                   \n";

    int id = build_shader( v_shader_str, f_shader_str );
    rstate.shader.id = id;
    rstate.shader.proj = find_uniform( id, "u_proj" );
    rstate.shader.color = find_uniform( id, "u_color" );

    glBindAttribLocation( id, 0, "a_pos" );
}

void render_init()
{
    const float triangle_data[] =
        { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };

    rstate.triangle_buffer.init();
    rstate.triangle_buffer.set( triangle_data, 3 );

    glm_mat4_identity( rstate.proj );

    shader_init();
}

void render()
{
    vec3 axis;
    axis[ 0 ] = 0.0f;
    axis[ 1 ] = 1.0f;
    axis[ 2 ] = 0.0f;

    glm_rotate_make( rstate.proj, state.render_time, axis );

    glClearColor( 0.1f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    vec4 color;
    color[ 0 ] = 1.0f;
    color[ 1 ] = 0.0f;
    color[ 2 ] = 0.0f;
    color[ 3 ] = 1.0f;

    glUseProgram( rstate.shader.id );

    set_uniform( rstate.shader.proj, rstate.proj );
    set_uniform( rstate.shader.color, color );

    rstate.triangle_buffer.render();
}
