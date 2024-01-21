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

struct sprite_t {
    rect_t rect;
    color_t color;
    float alpha = 1.0f;
    float rotation = 0.0f;

    void render();
};

// render state
struct {
    vbuffer_t quad_buffer;

    vbuffer_t fb_pos_buffer;
    vbuffer_t fb_uv_buffer;

    struct {
        int id;
        int proj;
        int model;
        int color;
    } shader1;

    struct {
        int id;
        int texture;
        int amount;
    } shader2;

    struct {
        int id;
        int proj;
        int model;
        int texture;
    } shader3;

    mat4 model;
    mat4 proj;

    float * ball_tail_x;
    float * ball_tail_y;
    int ball_tail_count;
    int ball_tail_front;

    // center camera
    float camera_x;
    float camera_y;
    float camera_zoom;
    float camera_rotate;

    framebuffer_t fb1;

    float abberation_timer;

} intern;

static void init_ball_tail()
{
    const int cap = 64;

    intern.ball_tail_count = cap;
    intern.ball_tail_front = 0;
    intern.ball_tail_x = new float[ cap ];
    intern.ball_tail_y = new float[ cap ];
}

static void push_ball_tail_helper( float x, float y )
{
    intern.ball_tail_front--;
    intern.ball_tail_front += intern.ball_tail_count;
    intern.ball_tail_front %= intern.ball_tail_count;

    intern.ball_tail_x[ intern.ball_tail_front ] = x;
    intern.ball_tail_y[ intern.ball_tail_front ] = y;
}

static void push_ball_tail()
{
    const int interp = 4;

    float last_x = intern.ball_tail_x[ intern.ball_tail_front ];
    float last_y = intern.ball_tail_y[ intern.ball_tail_front ];
    float new_x = state.ball_x;
    float new_y = state.ball_y;

    for ( int i = 0; i < interp; i++ ) {
        float t = (float) ( i + 1 ) / interp;
        float x = std::lerp( last_x, new_x, t );
        float y = std::lerp( last_y, new_y, t );
        push_ball_tail_helper( x, y );
    }
}

static void init_shader1()
{
    int id = build_shader(
        find_shader_string( "shader1_vertex" ),
        find_shader_string( "shader1_fragment" )
    );
    intern.shader1.id = id;
    intern.shader1.proj = find_uniform( id, "u_proj" );
    intern.shader1.model = find_uniform( id, "u_model" );
    intern.shader1.color = find_uniform( id, "u_color" );

    glBindAttribLocation( id, 0, "a_pos" );
}

static void init_shader2()
{
    int id = build_shader(
        find_shader_string( "shader2_vertex" ),
        find_shader_string( "shader2_fragment" )
    );
    intern.shader2.id = id;
    intern.shader2.texture = find_uniform( id, "u_texture" );
    intern.shader2.amount = find_uniform( id, "u_amount" );

    glBindAttribLocation( id, 0, "a_pos" );
    glBindAttribLocation( id, 1, "a_uv" );
}

static void init_shader3()
{
    int id = build_shader(
        find_shader_string( "shader3_vertex" ),
        find_shader_string( "shader3_fragment" )
    );
    intern.shader3.id = id;
    intern.shader3.proj = find_uniform( id, "u_proj" );
    intern.shader3.model = find_uniform( id, "u_model" );
    intern.shader3.texture = find_uniform( id, "u_texture" );

    glBindAttribLocation( id, 0, "a_pos" );
    glBindAttribLocation( id, 1, "a_uv" );
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

    glm_mat4_identity( intern.model );
    glm_translate( intern.model, translate );
    glm_scale( intern.model, scale );
    glm_translate( intern.model, vec3{ 0.5f, 0.5f, 0.0f } );
    glm_rotate( intern.model, rotation, axis );
    glm_translate( intern.model, vec3{ -0.5f, -0.5f, 0.0f } );

    set_uniform( intern.shader1.proj, intern.proj );
    set_uniform( intern.shader1.model, intern.model );
    set_uniform( intern.shader1.color, color4 );

    intern.quad_buffer.enable( 0 );
    glDrawArrays( GL_TRIANGLES, 0, intern.quad_buffer.element_count );
}

static void update_camera()
{
    float zoom = intern.camera_zoom;
    glm_ortho(
        0.5f * zoom * -hardware_width(),
        0.5f * zoom * hardware_width(),
        0.5f * zoom * hardware_height(),
        0.5f * zoom * -hardware_height(),
        0.0f,
        1000.0f,
        intern.proj
    );

    float rot = intern.camera_rotate;

    mat4 view;
    glm_look(
        vec3{ intern.camera_x, intern.camera_y, 0.0f }, // eye
        vec3{ 0.0f, 0.0f, -1.0f },                      // dir
        vec3{ sin( rot ), cos( rot ), 0.0f },           // up
        view
    );
    glm_mat4_mul( intern.proj, view, intern.proj );
}

void render_init()
{
    float quad_data[ 18 ];
    rect_t{ 0.0f, 0.0f, 1.0f, 1.0f }.vertices( quad_data );

    float fb_pos_data[ 12 ];
    rect_t{ -1.0f, -1.0f, 2.0f, 2.0f }.vertices_2d( fb_pos_data );

    float fb_uv_data[ 12 ];
    rect_t{ 0.0f, 0.0f, 1.0f, 1.0f }.vertices_2d( fb_uv_data );

    // init vertex buffers

    intern.quad_buffer.init( 3 );
    intern.quad_buffer.set( quad_data, 6 );

    intern.fb_pos_buffer.init( 2 );
    intern.fb_pos_buffer.set( fb_pos_data, 6 );

    intern.fb_uv_buffer.init( 2 );
    intern.fb_uv_buffer.set( fb_uv_data, 6 );

    // init frame buffers

    intern.fb1.init( hardware_width(), hardware_height() );

    // init shaders

    init_shader1();
    init_shader2();
    init_shader3();

    // init misc

    init_ball_tail();
    intern.camera_zoom = 1.0f;
    glm_mat4_identity( intern.model );

    // init gl state

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
    ball.rotation = state.render_time * 30.0f * ( state.ball_speed / 200.0f );

    ball.render();

    for ( int i = 0; i < intern.ball_tail_count; i++ ) {
        float t = (float) i / intern.ball_tail_count;
        float radius = ( 1.0f - ( t * 0.5f ) ) * state.ball_radius;
        int j = ( i + intern.ball_tail_front ) % intern.ball_tail_count;

        ball.rect = {
            intern.ball_tail_x[ j ] - state.ball_radius,
            intern.ball_tail_y[ j ] - state.ball_radius,
            2 * radius,
            2 * radius,
        };

        ball.alpha = 1.0f - t;
        // ball.rotation += 0.1f;
        ball.render();
    }
}

static void render_room()
{
    int outline_width = 5;

    sprite_t bg_fill;
    bg_fill.rect = { 0.0f, 0.0f, (float) state.room_w, (float) state.room_h };
    bg_fill.color = { 0.0f, 0.0f, 0.0f };

    sprite_t bg_outline;
    bg_outline.rect = bg_fill.rect;
    bg_outline.rect.margin( -outline_width );
    bg_outline.rect.h -= outline_width;
    bg_outline.color = color_white;

    bg_outline.render();

    sprite_t wall_mask;
    wall_mask.rect = {
        state.room_w - 50.0f,
        (float) -outline_width,
        50.0f + outline_width,
        50.0f };

    if ( state.room_state == 0 ) {
        wall_mask.color = color_white;
    }

    if ( state.room_state == 1 ) {
        wall_mask.color = color_red;
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

static void tick_abberation()
{
    if ( state.ball_hit ) intern.abberation_timer = 1.0f;

    if ( intern.abberation_timer <= 0.0f ) return;

    intern.abberation_timer -= 5.0f * state.render_step;

    if ( intern.abberation_timer <= 0.0f ) {
        intern.abberation_timer = 0.0f;
    }
}

void render()
{
    glBindFramebuffer( GL_FRAMEBUFFER, intern.fb1.id );

    {
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );

        glUseProgram( intern.shader1.id );

        intern.camera_x = state.ball_x;
        intern.camera_y = state.ball_y;
        intern.camera_zoom -= state.render_step * 0.001f;
        intern.camera_rotate += 0.03f * state.render_step;

        update_camera();

        push_ball_tail();

        render_room();
        render_blocks();
        render_paddle();
        render_ball();
        // render_black();
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    {
        tick_abberation();

        glUseProgram( intern.shader2.id );

        glBindTexture( GL_TEXTURE_2D, intern.fb1.texture );
        set_uniform( intern.shader2.texture, 0 );

        float x = sin( intern.abberation_timer * M_PI );
        set_uniform( intern.shader2.amount, x );
        set_uniform( intern.shader2.amount, 1.0f );

        intern.fb_pos_buffer.enable( 0 );
        intern.fb_uv_buffer.enable( 1 );
        glDrawArrays( GL_TRIANGLES, 0, 6 );
    }
}
