#pragma once

#include "cglm/types.h"

struct pos_buffer_t {
    int pos_buffer;
    int count;

    void init();
    void set( const float * buffer, int count );
    void bind();
    void render();
};

struct pos_uv_buffer_t {
    int pos_buffer;
    int uv_buffer;
    int count;

    void init();
    void set( const float * pos_buffer, const float * uv_buffer, int count );
    void bind();
    void render();
};

int build_shader( const char * vertex_string, const char * fragment_string );

int find_uniform( int shader, const char * uniform_name );

void set_uniform( int uniform, float ( &v )[ 3 ] );
void set_uniform( int uniform, float ( &v )[ 4 ] );
void set_uniform( int uniform, vec4 ( &m )[ 4 ] );
