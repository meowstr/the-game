#include "render.hpp"

#include "color.hpp"
#include "hardware.hpp"
#include "render_utils.hpp"
#include "shape.hpp"
#include "state.hpp"

#include "cglm/affine.h"
#include "cglm/cam.h"
#include "cglm/mat4.h"

#include <GLES2/gl2.h>
#include <math.h>
#include <vector>

// render state
struct {
    pos_buffer_t triangle_buffer;
    pos_buffer_t quad_buffer;

    struct {
        int id;
        int proj;
        int model;
        int color;
    } shader;

    mat4 model;
    mat4 proj;

} rstate;

struct sprite_t {
    rect_t rect;
    color_t color;
    float rotation = 0.0f;

    void render();
};

static void shader_init()
{
    const char * v_shader_str = "#version 100                        \n"
                                "attribute vec3 a_pos;               \n"
                                "uniform mat4 u_proj;                \n"
                                "uniform mat4 u_model;                \n"
                                "void main()                         \n"
                                "{                                   \n"
                                "   vec4 pos = vec4(a_pos, 1.0);     \n"
                                "   gl_Position = u_proj *           \n"
                                "                 u_model * pos;     \n"
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
    rstate.shader.model = find_uniform( id, "u_model" );
    rstate.shader.color = find_uniform( id, "u_color" );

    glBindAttribLocation( id, 0, "a_pos" );
}

void sprite_t::render()
{
    vec4 color4;
    color4[ 0 ] = color.r;
    color4[ 1 ] = color.g;
    color4[ 2 ] = color.b;
    color4[ 3 ] = 1.0f;

    vec3 scale;
    scale[ 0 ] = roundf( rect.w );
    scale[ 1 ] = roundf( rect.h );
    scale[ 2 ] = 1.0f;

    vec3 translate;
    translate[ 0 ] = roundf( rect.x );
    translate[ 1 ] = roundf( rect.y );
    translate[ 2 ] = 0.0f;

    vec3 axis;
    axis[ 0 ] = 0.0f;
    axis[ 1 ] = 0.0f;
    axis[ 2 ] = 1.0f;

    glm_mat4_identity( rstate.model );
    glm_translate( rstate.model, translate );
    glm_scale( rstate.model, scale );
    glm_translate( rstate.model, vec3{ 0.5f, 0.5f, 0.0f } );
    glm_rotate( rstate.model, rotation, axis );
    glm_translate( rstate.model, vec3{ -0.5f, -0.5f, 0.0f } );

    set_uniform( rstate.shader.proj, rstate.proj );
    set_uniform( rstate.shader.model, rstate.model );
    set_uniform( rstate.shader.color, color4 );

    rstate.quad_buffer.render();
}

void render_init()
{
    const float triangle_data[] =
        { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };

    float quad_data[ 18 ];
    rect_t{ 0.0f, 0.0f, 1.0f, 1.0f }.vertices( quad_data );

    rstate.triangle_buffer.init();
    rstate.triangle_buffer.set( triangle_data, 3 );

    rstate.quad_buffer.init();
    rstate.quad_buffer.set( quad_data, 6 );

    glm_ortho(
        0.0f,
        hardware_width(),
        hardware_height(),
        0.0f,
        0.0f,
        1000.0f,
        rstate.proj
    );

    glm_mat4_identity( rstate.model );

    shader_init();
}

void render()
{
    glClearColor( 0.1f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    glUseProgram( rstate.shader.id );

    for ( rect_t & rect : state.block_rect_list ) {
        sprite_t s;
        s.rect = rect;
        s.color = color_red;
        s.render();
    }

    sprite_t paddle;
    paddle.rect = state.paddle_rect;
    paddle.color = color_red;
    paddle.render();

    rect_t ball_rect;
    ball_rect = {
        state.ball_x - state.ball_radius,
        state.ball_y - state.ball_radius,
        2 * state.ball_radius,
        2 * state.ball_radius,
    };

    sprite_t ball;
    ball.rect = ball_rect;
    ball.color = color_yellow;
    ball.rotation = state.render_time * 10.0f;
    ball.render();
}
