#pragma once

#include "shape.hpp"

#include <vector>

struct state_t {
    float tick_time;
    float render_time;

    float tick_step;
    float render_step;

    std::vector< rect_t > block_rect_list;

    rect_t paddle_rect;

    float ball_x;
    float ball_y;
    float ball_radius;
    float ball_vx;
    float ball_vy;

    float throw_timer;
};

extern state_t state;
