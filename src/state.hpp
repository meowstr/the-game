#pragma once

#include "shape.hpp"

template < typename T > void array_swap_last( T * arr, int count, int index )
{
    arr[ index ] = arr[ count - 1 ];
}

struct state_t {
    float tick_time;
    float render_time;

    float tick_step;
    float render_step;

    // block table
    rect_t * block_rect_list;
    float * block_timer_list;
    int * block_state_list;
    int block_count;

    rect_t paddle_rect;
    int paddle_touched;

    float ball_x;
    float ball_y;
    float ball_radius;

    float ball_dir_x;
    float ball_dir_y;
    float ball_speed;

    float throw_timer;

    // current room
    int room_w;
    int room_h;

    int room_state;

    int ball_hit;
};

extern state_t state;
