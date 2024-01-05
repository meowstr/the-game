#include "hardware.hpp"
#include "logging.hpp"
#include "render.hpp"
#include "state.hpp"

#include <algorithm>
#include <float.h>
#include <math.h>

#include <GLES2/gl2.h>

static void reset_ball();

static float clamp( float x, float min, float max )
{
    if ( x < min ) return min;
    if ( x > max ) return max;
    return x;
}

static void tick_paddle()
{
    float speed = 500.0f;
    state.paddle_rect.x += speed * hardware_x_axis();
    state.paddle_rect.x = clamp(
        state.paddle_rect.x,
        5.0f,
        hardware_width() - state.paddle_rect.w - 5.0f
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
        state.ball_vx *= -500;
    }

    // right edge
    if ( closest == dx2 ) {
        state.ball_x = rect.x + rect.w;
        state.ball_vx *= -500;
    }

    // top edge
    if ( closest == dy1 ) {
        state.ball_y = rect.y;
        state.ball_vy *= -500;
    }

    // bottom edge
    if ( closest == dy2 ) {
        state.ball_y = rect.y + rect.h;
        state.ball_vy *= -500;
    }

    return 1;
}

static void tick_ball()
{
    if ( state.throw_timer > 0.0f ) {
        state.throw_timer -= state.tick_step;
        state.ball_x = state.paddle_rect.center_x();
        state.ball_y = state.paddle_rect.y - state.ball_radius - 2.0f;
        return;
    }

    state.ball_x += state.ball_vx * state.tick_step;
    state.ball_y += state.ball_vy * state.tick_step;

    if ( state.ball_x < 0.0f ) {
        state.ball_x = 0.0f;
        state.ball_vx *= -1;
    }

    if ( state.ball_x > hardware_width() ) {
        state.ball_x = hardware_width();
        state.ball_vx *= -1;
    }

    if ( state.ball_y < 0.0f ) {
        state.ball_y = 0.0f;
        state.ball_vy *= -1;
    }

    if ( state.ball_y > hardware_height() ) {
        reset_ball();
    }

    collide_ball( state.paddle_rect );

    for ( int i = 0; i < std::ssize( state.block_rect_list ); i++ ) {
        int collided = collide_ball( state.block_rect_list[ i ] );

        if ( collided ) {
            std::swap(
                state.block_rect_list[ i ],
                state.block_rect_list.back()
            );
            state.block_rect_list.pop_back();
            break;
        }
    }
}

static void loop()
{
    state.tick_time += 1.0f / 60.0f;
    state.render_time += 1.0f / 60.0f;

    state.tick_step = 1.0f / 60.0f;
    state.render_step = 1.0f / 60.0f;

    tick_paddle();
    tick_ball();

    render();
}

static void reset_ball()
{
    state.ball_vx = 250.0f;
    state.ball_vy = -250.0f;

    state.throw_timer = 2.0f;
}

static void init()
{
    int cols = 10;
    int rows = 6;

    int width = hardware_width() / cols;
    int height = 30;

    for ( int r = 0; r < rows; r++ ) {
        for ( int c = 0; c < cols; c++ ) {
            rect_t rect{
                (float) c * width,
                (float) r * height,
                (float) width,
                (float) height,
            };

            rect.margin( 5 );

            state.block_rect_list.push_back( rect );
        }
    }

    state.paddle_rect = { 0.0f, hardware_height() - 20.0f, 100.0f, 20.0f };
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

    return 0;
}
