#include "render_utils.hpp"

#include "logging.hpp"

#include <GLES2/gl2.h>

int create_shader( int * out, int type, const char * source )
{
    int shader;
    int compiled;

    // DEBUG_LOG("Shader source:\n%s", source);

    shader = glCreateShader( type );

    if ( shader == 0 ) {
        ERROR_LOG( "failed to create shader" );
        return 1;
    }

    glShaderSource( shader, 1, &source, nullptr );

    glCompileShader( shader );

    glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

    if ( !compiled ) {
        int info_len = 0;

        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &info_len );

        if ( info_len > 1 ) {
            char * info_log = new char[ sizeof( char ) * info_len ];

            glGetShaderInfoLog( shader, info_len, nullptr, info_log );
            ERROR_LOG( "failed to compile shader:\n%s", info_log );

            delete[] info_log;
        } else {
            ERROR_LOG( "failed to compile shader (no error message)" );
        }

        glDeleteShader( shader );
        return 1;
    }

    *out = shader;

    return 0;
}

int create_shader_program( int * out, int * shaders, int count )
{
    int program;
    int linked;

    program = glCreateProgram();

    if ( program == 0 ) {
        ERROR_LOG( "failed to create shader program" );
        return 1;
    }

    for ( int i = 0; i < count; i++ ) {
        glAttachShader( program, shaders[ i ] );
    }

    glLinkProgram( program );

    glGetProgramiv( program, GL_LINK_STATUS, &linked );

    if ( !linked ) {
        int info_len = 0;

        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &info_len );

        if ( info_len > 1 ) {
            char * info_log = new char[ sizeof( char ) * info_len ];

            glGetProgramInfoLog( program, info_len, nullptr, info_log );
            ERROR_LOG( "failed to compile program:\n%s", info_log );

            delete[] info_log;
        } else {
            ERROR_LOG( "failed to compile program (no error message)" );
        }

        glDeleteProgram( program );
        return 1;
    }

    *out = program;

    return 0;
}

int build_shader( const char * vertex_source, const char * fragment_source )
{
    int shaders[ 2 ];
    int error;
    int program = -1;

    error = create_shader( shaders + 0, GL_VERTEX_SHADER, vertex_source );
    if ( error ) {
        return 1;
    }

    error = create_shader( shaders + 1, GL_FRAGMENT_SHADER, fragment_source );
    if ( error ) {
        goto cleanup_shader1;
    }

    error = create_shader_program( &program, shaders, 2 );
    if ( error ) {
        goto cleanup_shader2;
    }

cleanup_shader2:
    glDeleteShader( shaders[ 1 ] );
cleanup_shader1:
    glDeleteShader( shaders[ 0 ] );

    return program;
}

int find_uniform( int shader, const char * uniform_name )
{
    int location = glGetUniformLocation( shader, uniform_name );
    if ( location < 0 ) {
        ERROR_LOG( "failed to find uniform '%s' in shader", uniform_name );
    }
    return location;
}

void pos_buffer_t::init()
{
    unsigned int temp_pos_buffer;
    unsigned int temp_color_buffer;
    glGenBuffers( 1, &temp_pos_buffer );
    glGenBuffers( 1, &temp_color_buffer );

    pos_buffer = temp_pos_buffer;
    color_buffer = temp_color_buffer;
    count = 0;
}

void pos_buffer_t::set(
    const float * pos_data,
    const float * color_data,
    int new_count
)
{
    glBindBuffer( GL_ARRAY_BUFFER, pos_buffer );
    glBufferData(
        GL_ARRAY_BUFFER,                 // type
        new_count * 3 * sizeof( float ), // size in bytes
        pos_data,                        // data pointer
        GL_DYNAMIC_DRAW                  // render strategy
    );

    glBindBuffer( GL_ARRAY_BUFFER, color_buffer );
    glBufferData(
        GL_ARRAY_BUFFER,                 // type
        new_count * 3 * sizeof( float ), // size in bytes
        color_data,                        // data pointer
        GL_DYNAMIC_DRAW                  // render strategy
    );

    count = new_count;
}

void pos_buffer_t::bind()
{
    glBindBuffer( GL_ARRAY_BUFFER, pos_buffer );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer(
        0,                     // attrib index
        3,                     // element size
        GL_FLOAT,              // type
        GL_FALSE,              // normalize
        3 * sizeof( GLfloat ), // stride
        (void *) ( 0 )         // offset
    );

    glBindBuffer( GL_ARRAY_BUFFER, color_buffer );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer(
        1,                     // attrib index
        3,                     // element size
        GL_FLOAT,              // type
        GL_FALSE,              // normalize
        3 * sizeof( GLfloat ), // stride
        (void *) ( 0 )         // offset
    );
}

void pos_buffer_t::render()
{
    if ( count <= 0 ) return;

    bind();
    glDrawArrays( GL_TRIANGLES, 0, count );
}

void pos_uv_buffer_t::init()
{
    unsigned int temp_pos_buffer;
    unsigned int temp_uv_buffer;
    glGenBuffers( 1, &temp_pos_buffer );
    glGenBuffers( 1, &temp_uv_buffer );

    pos_buffer = temp_pos_buffer;
    uv_buffer = temp_uv_buffer;
    count = 0;
}

void pos_uv_buffer_t::set(
    const float * pos_buffer,
    const float * uv_buffer,
    int count
)
{
    // TODO
}

void pos_uv_buffer_t::bind()
{
    // TODO
}

void pos_uv_buffer_t::render()
{
    // TODO
}

void set_uniform_vec3( int uniform, vec3 v )
{
    glUniform3fv( uniform, 1, v );
}

void set_uniform_vec4( int uniform, vec4 v )
{
    glUniform4fv( uniform, 1, v );
}

void set_uniform_mat4( int uniform, mat4 m )
{
    glUniformMatrix4fv( uniform, 1, GL_FALSE, (float *) m );
}
