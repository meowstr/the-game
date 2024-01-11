#pragma once

using loop_function_t = void ( * )();

int hardware_init();
void hardware_destroy();
void hardware_set_loop( loop_function_t step );

int hardware_width();
int hardware_height();

float hardware_x_axis();

float hardware_time();
