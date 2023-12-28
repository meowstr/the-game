#pragma once

#include "cglm/types.h"

struct pos_buffer_t {
    int pos_buffer;
    int count;

    void init();
    void set( float * buffer, int count );
    void bind();
    void render();
};

struct pos_uv_buffer_t {
    int pos_buffer;
    int uv_buffer;
    int count;

    void init();
    void set( float * pos_buffer, float * uv_buffer, int count );
    void bind();
    void render();
};

int build_shader( const char * vertex_string, const char * fragment_string );

int find_uniform( int shader, const char * uniform_name );

void set_uniform_vec3( int uniform, vec3 v );
void set_uniform_vec4( int uniform, vec4 v );
void set_uniform_mat4( int uniform, mat4 v );

