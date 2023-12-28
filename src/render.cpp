#include "render.hpp"

#include "hardware.hpp"
#include "render_utils.hpp"
#include "shape.hpp"
#include "state.hpp"

#include "cglm/affine.h"
#include "cglm/cam.h"
#include "cglm/mat4.h"

#include <GLES2/gl2.h>

#include <vector>

// render state
struct {
    pos_buffer_t triangle_buffer;
    pos_buffer_t quad_buffer;

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

static void build_level()
{
    std::vector< rect_t > rects;
    std::vector< float > vertex_data;

    int num_cols = 10;
    int num_rows = 3;

    float width = (float) hardware_width() / num_cols;
    float height = 30.0f;

    for ( int r = 0; r < num_rows; r++ ) {
        for ( int c = 0; c < num_cols; c++ ) {
            rect_t rect{
                c * width,
                r * height,
                width,
                height,
            };

            rect.margin( 5 );
            rects.push_back( rect );
        }
    }

    for ( auto & rect : rects ) {
        int i = std::ssize( vertex_data );
        vertex_data.resize( i + 18 );
        rect.vertices( &vertex_data[ i ] );
    }

    rstate.quad_buffer.set( vertex_data.data(), std::ssize( vertex_data ) / 3 );
}

void render_init()
{
    const float triangle_data[] =
        { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };
    float quad_data[ 18 ];

    rstate.triangle_buffer.init();
    rstate.triangle_buffer.set( triangle_data, 3 );

    rstate.quad_buffer.init();
    build_level();

    glm_ortho(
        0.0f,
        hardware_width(),
        hardware_height(),
        0.0f,
        0.0f,
        1000.0f,
        rstate.proj
    );

    shader_init();
}

void render()
{
    vec3 axis;
    axis[ 0 ] = 0.0f;
    axis[ 1 ] = 1.0f;
    axis[ 2 ] = 0.0f;

    //glm_rotate_make( rstate.proj, state.render_time, axis );

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

    rstate.quad_buffer.render();
}
