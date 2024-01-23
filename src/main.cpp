#include "hardware.hpp"
#include "logging.hpp"
#include "render.hpp"
#include "state.hpp"

#include <algorithm>
#include <float.h>
#include <math.h>

static void reset_ball();

static void init_block_table()
{
    const int cap = 128;

    state.block_rect_list = new rect_t[ cap ];
    state.block_timer_list = new float[ cap ];
    state.block_state_list = new int[ cap ];
}

static void remove_block( int index )
{
    array_swap_last( state.block_rect_list, state.block_count, index );
    array_swap_last( state.block_timer_list, state.block_count, index );
    array_swap_last( state.block_state_list, state.block_count, index );
    state.block_count--;
}

static float clamp( float x, float min, float max )
{
    if ( x < min ) return min;
    if ( x > max ) return max;
    return x;
}

static void tick_paddle()
{
    float speed = 10.0f;
    state.paddle_rect.x += speed * hardware_x_axis();
    state.paddle_rect.x = clamp(
        state.paddle_rect.x,
        5.0f,
        state.room_w - state.paddle_rect.w - 5.0f
    );
}

static int collide_ball( rect_t rect )
{
    float dx1 = state.ball_x - rect.x;
    float dx2 = rect.x + rect.w - state.ball_x;

    float dy1 = state.ball_y - rect.y;
    float dy2 = rect.y + rect.h - state.ball_y;

    if ( dx1 <= 0 ) return 0;
    if ( dx2 <= 0 ) return 0;
    if ( dy1 <= 0 ) return 0;
    if ( dy2 <= 0 ) return 0;

    float closest = FLT_MAX;

    for ( float f : { dx1, dx2, dy1, dy2 } ) {
        closest = fmin( f, closest );
    }

    // left edge
    if ( closest == dx1 ) {
        state.ball_x = rect.x;
        state.ball_dir_x *= -1;
    }

    // right edge
    if ( closest == dx2 ) {
        state.ball_x = rect.x + rect.w;
        state.ball_dir_x *= -1;
    }

    // top edge
    if ( closest == dy1 ) {
        state.ball_y = rect.y;
        state.ball_dir_y *= -1;
    }

    // bottom edge
    if ( closest == dy2 ) {
        state.ball_y = rect.y + rect.h;
        state.ball_dir_y *= -1;
    }

    return 1;
}

static void tick_blocks()
{
    for ( int i = 0; i < state.block_count; i++ ) {
        int & b_state = state.block_state_list[ i ];
        float & b_timer = state.block_timer_list[ i ];
        rect_t & b_rect = state.block_rect_list[ i ];

        if ( b_state != 2 ) continue;

        const float speed = 2.0f;

        b_timer -= state.tick_step * speed;
        b_rect.margin( -100.0f * state.tick_step );

        if ( b_timer <= 0.0f ) {
            remove_block( i );
            break;
        }
    }
}

static int collide_corner()
{
    if ( !( state.ball_x > state.room_w - 50.0f && state.ball_y < 50.0f ) )
        return 0;

    if ( state.room_state == 0 ) {
        state.room_state = 1;
        return 0;
    }

    if ( state.room_state == 1 ) {
        state.room_state = 2;
        return 0;
    }

    if ( state.room_state == 2 ) {
        state.room_state = 3;
        return 0;
    }

    if ( state.room_state == 3 ) {
        return 1;
    }

    return 0;
}

static void tick_ball_asymmptote()
{
    if ( state.block_count != 1 ) return;

    float dx = state.block_rect_list[ 0 ].x - state.ball_x;
    float dy = state.block_rect_list[ 0 ].y - state.ball_y;

    float dist = sqrt( dx * dx + dy * dy );

    state.ball_speed = clamp( ( dist - 20.0f ) * 1.0f, 0.0f, 200.0f );
}

static void tick_ball()
{
    state.ball_hit = 0;

    tick_ball_asymmptote();

    // do funny
    if ( state.throw_timer > 0.0f ) {
        state.throw_timer -= state.tick_step;
        state.ball_x = state.paddle_rect.center_x();
        state.ball_y = state.paddle_rect.y - state.ball_radius - 2.0f;
        return;
    }

    state.ball_x += state.ball_speed * state.ball_dir_x * state.tick_step;
    state.ball_y += state.ball_speed * state.ball_dir_y * state.tick_step;

    if ( state.ball_x < 0.0f ) {
        if ( collide_corner() ) return;
        state.ball_x = 0.0f;
        state.ball_dir_x *= -1;
    }

    if ( state.ball_x > state.room_w ) {
        if ( collide_corner() ) return;
        state.ball_x = state.room_w;
        state.ball_dir_x *= -1;
    }

    if ( state.ball_y < 0.0f ) {
        if ( collide_corner() ) return;
        state.ball_y = 0.0f;
        state.ball_dir_y *= -1;
    }

    if ( state.ball_y > state.room_h ) {
        if ( collide_corner() ) return;
        reset_ball();
    }

    state.paddle_touched = collide_ball( state.paddle_rect );

    if ( state.paddle_touched ) {
        state.ball_hit = 1;
    }

    for ( int i = 0; i < state.block_count; i++ ) {
        int & b_state = state.block_state_list[ i ];

        if ( b_state == 2 ) continue;

        int collided = collide_ball( state.block_rect_list[ i ] );

        if ( collided ) {
            state.ball_hit = 1;

            if ( b_state == 0 ) {
                b_state = 1;
                break;
            }
            if ( b_state == 1 ) {
                b_state = 2;
                break;
            }
        }
    }

    if ( state.ball_hit ) {
        hardware_rumble();
    }
}

static void loop()
{
    float time = hardware_time();

    state.tick_step = fmin( time - state.tick_time, 1 / 60.0f );
    state.render_step = state.tick_step;

    state.tick_time = time;
    state.render_time = time;

    tick_paddle();
    tick_ball();
    tick_blocks();

    render();
}

static void reset_ball()
{
    state.ball_dir_x = 1.4f;
    state.ball_dir_y = -1.4f;
    state.ball_speed = 200.0f;

    state.throw_timer = 2.0f;
}

static void init()
{
    init_block_table();

    state.room_w = 512;
    state.room_h = 512;

    int cols = 10;
    int rows = 6;

    int width = state.room_w / cols;
    int height = 30;

    for ( int r = 0; r < rows; r++ ) {
        for ( int c = 0; c < cols; c++ ) {
            rect_t rect{
                (float) c * width,
                (float) r * height,
                (float) width,
                (float) height,
            };

            rect.margin( 8 );

            int i = state.block_count;
            state.block_count++;

            state.block_rect_list[ i ] = rect;
            state.block_timer_list[ i ] = 1.0f;
            state.block_state_list[ i ] = 0;
        }
    }

    state.paddle_rect = { 0.0f, state.room_h - 20.0f, 100.0f, 20.0f };
    state.paddle_rect.margin( 5 );

    state.ball_radius = 5.0f;

    reset_ball();
}

int main()
{
    INFO_LOG( "meow" );

    hardware_init();

    init();

    render_init();

    hardware_set_loop( loop );

    hardware_destroy();

    return 0;
}
