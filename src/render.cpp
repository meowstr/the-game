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

    float * ball_tail_x;
    float * ball_tail_y;
    int ball_tail_count;
    int ball_tail_front;

} rstate;

struct sprite_t {
    rect_t rect;
    color_t color;
    float alpha = 1.0f;
    float rotation = 0.0f;

    void render();
};

static void init_ball_tail()
{
    const int cap = 64;

    rstate.ball_tail_count = cap;
    rstate.ball_tail_front = 0;
    rstate.ball_tail_x = new float[ cap ];
    rstate.ball_tail_y = new float[ cap ];
}

static void push_ball_tail_helper( float x, float y )
{
    rstate.ball_tail_front--;
    rstate.ball_tail_front += rstate.ball_tail_count;
    rstate.ball_tail_front %= rstate.ball_tail_count;

    rstate.ball_tail_x[ rstate.ball_tail_front ] = x;
    rstate.ball_tail_y[ rstate.ball_tail_front ] = y;
}

static void push_ball_tail()
{
    const int interp = 4;

    float last_x = rstate.ball_tail_x[ rstate.ball_tail_front ];
    float last_y = rstate.ball_tail_y[ rstate.ball_tail_front ];
    float new_x = state.ball_x;
    float new_y = state.ball_y;

    for ( int i = 0; i < interp; i++ ) {
        float t = (float) ( i + 1 ) / interp;
        float x = std::lerp( last_x, new_x, t );
        float y = std::lerp( last_y, new_y, t );
        push_ball_tail_helper( x, y );
    }
}

static void init_shader()
{
    const char * v_shader_str = "#version 100                        \n"
                                "attribute vec3 a_pos;               \n"
                                "uniform mat4 u_proj;                \n"
                                "uniform mat4 u_model;               \n"
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
    color4[ 3 ] = alpha;

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

static void update_camera()
{
    // glm_ortho(
    //     0.0f,
    //     hardware_width(),
    //     hardware_height(),
    //     0.0f,
    //     0.0f,
    //     1000.0f,
    //     rstate.proj
    //);

    glm_ortho(
        0.5f * -hardware_width(),
        0.5f * hardware_width(),
        0.5f * hardware_height(),
        0.5f * -hardware_height(),
        0.0f,
        1000.0f,
        rstate.proj
    );

    float dx = state.camera_x - hardware_width() * 0.5f;
    float dy = state.camera_y - hardware_height() * 0.5f;

    static float rotation = 0.0f;
    rotation += 0.01f * state.render_step;

    mat4 view;
    glm_look(
        vec3{ state.camera_x, state.camera_y, 0.0f },   // eye
        vec3{ 0.0f, 0.0f, -1.0f },                      // dir
        vec3{ sin( rotation ), cos( rotation ), 0.0f }, // up
        view
    );
    glm_mat4_mul( rstate.proj, view, rstate.proj );
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

    glm_mat4_identity( rstate.model );

    init_shader();

    init_ball_tail();

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); // blend alpha
}

static void render_blocks()
{
    for ( int i = 0; i < state.block_count; i++ ) {
        sprite_t s;

        s.rect = state.block_rect_list[ i ];

        int b_state = state.block_state_list[ i ];
        if ( b_state == 0 ) {
            s.color = color_red;
        }
        if ( b_state == 1 ) {
            s.color = color_yellow;
        }
        if ( b_state == 2 ) {
            s.color = color_yellow;
            s.alpha = state.block_timer_list[ i ];
        }

        s.render();
    }
}

static void render_paddle()
{
    sprite_t paddle;
    paddle.rect = state.paddle_rect;
    if ( state.paddle_touched ) {
        paddle.color = color_yellow;
    } else {
        paddle.color = color_red;
    }
    paddle.render();
}

static void render_ball()
{
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
    ball.rotation = state.render_time * 30.0f;

    ball.render();

    for ( int i = 0; i < rstate.ball_tail_count; i++ ) {
        float t = (float) i / rstate.ball_tail_count;
        float radius = ( 1.0f - ( t * 0.5f ) ) * state.ball_radius;
        int j = ( i + rstate.ball_tail_front ) % rstate.ball_tail_count;

        ball.rect = {
            rstate.ball_tail_x[ j ] - state.ball_radius,
            rstate.ball_tail_y[ j ] - state.ball_radius,
            2 * radius,
            2 * radius,
        };

        ball.alpha = 1.0f - t;
        ball.rotation += 0.1f;
        ball.render();
    }
}

static void render_room()
{
    int outline_width = 5;

    sprite_t bg_fill;
    bg_fill.rect = { 0.0f, 0.0f, (float) state.room_w, (float) state.room_h };
    bg_fill.color = { 0.1f, 0.0f, 0.0f };

    sprite_t bg_outline;
    bg_outline.rect = bg_fill.rect;
    bg_outline.rect.margin( -outline_width );
    bg_outline.rect.h -= outline_width;
    bg_outline.color = { 1.0f, 0.0f, 0.0f };

    bg_outline.render();

    sprite_t wall_mask;
    wall_mask.rect = {
        state.room_w - 50.0f,
        (float) -outline_width,
        50.0f + outline_width,
        50.0f };

    if ( state.room_state == 0 ) {
        wall_mask.color = color_red;
    }

    if ( state.room_state == 1 ) {
        wall_mask.color = color_yellow;
    }

    if ( state.room_state == 2 ) {
        wall_mask.color = color_yellow;
    }

    if ( state.room_state == 3 ) {
        wall_mask.color = color_black;
    }

    wall_mask.render();

    bg_fill.render();
}

static void render_black()
{
    sprite_t fill;
    fill.rect = {
        -5.0f,
        -5.0f,
        (float) hardware_width() + 10.0f,
        (float) hardware_height() + 10.0f };
    fill.color = color_black;
    fill.alpha = fmax( 0.0f, sin( state.render_time ) );
    fill.render();
}

void render()
{
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    glUseProgram( rstate.shader.id );

    state.camera_x = state.ball_x;
    state.camera_y = state.ball_y;

    update_camera();
    push_ball_tail();

    render_room();
    render_blocks();
    render_paddle();
    render_ball();
    render_black();
}
