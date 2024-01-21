#include "render_utils.hpp"

#include "logging.hpp"
#include "res.h"

#include <GLES2/gl2.h>

#include <sstream>
#include <string>
#include <vector>

static int match_shader_line( const char * line, const char * name )
{
    char buffer[ 128 ];
    snprintf( buffer, 128, "#shader %s", name );

    return strncmp( line, buffer, strlen( buffer ) ) == 0;
}

static int match_any_shader_line( const char * line )
{
    const char * buffer = "#shader";
    return strncmp( line, buffer, strlen( buffer ) ) == 0;
}

// TODO: make this less c++ dependent?
const char * find_shader_string( const char * name )
{
    const unsigned char * shaders = src_shaders_glsl;
    int len = sizeof( src_shaders_glsl );

    // split into lines
    std::vector< std::string > lines;

    {
        std::string line;
        for ( int i = 0; i < len; i++ ) {
            if ( shaders[ i ] == '\n' ) {
                lines.push_back( line );
                line = "";
            } else {
                line += shaders[ i ];
            }
        }
    }

    // find shader line
    int matched_line = -1;
    for ( int i = 0; i < std::ssize( lines ); i++ ) {
        if ( match_shader_line( lines[ i ].c_str(), name ) ) {
            matched_line = i;
            break;
        }
    }

    // no match
    if ( matched_line == -1 ) {
        ERROR_LOG( "failed to find shader: %s", name );
        return "";
    }

    // match lines
    std::vector< std::string > matched_lines;
    for ( int i = matched_line + 1; i < std::ssize( lines ); i++ ) {
        // stop when we get to another shader
        if ( match_any_shader_line( lines[ i ].c_str() ) ) break;
        matched_lines.push_back( lines[ i ] );
    }

    // combine lines into one string
    std::stringstream ss;
    for ( std::string & line : matched_lines ) {
        ss << line << '\n';
    }

    // TODO: lol yea i know
    std::string * str = new std::string( ss.str() );

    return str->c_str();
}

static int create_shader( int * out, int type, const char * source )
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

static int create_shader_program( int * out, int * shaders, int count )
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

void vbuffer_t::init( int new_element_size )
{
    unsigned int new_buffer;
    glGenBuffers( 1, &new_buffer );

    buffer = new_buffer;
    element_size = new_element_size;
    element_count = 0;
}

void vbuffer_t::set( const float * new_data, int new_element_count )
{
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData(
        GL_ARRAY_BUFFER,                                    // type
        new_element_count * element_size * sizeof( float ), // size in bytes
        new_data,                                           // data pointer
        GL_DYNAMIC_DRAW                                     // render strategy
    );

    element_count = new_element_count;
}

void vbuffer_t::enable( int attrib_index )
{
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glEnableVertexAttribArray( attrib_index );
    glVertexAttribPointer(
        attrib_index,                     // attrib index
        element_size,                     // element size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalize
        element_size * sizeof( GLfloat ), // stride
        (void *) ( 0 )                    // offset
    );
}

int find_uniform( int shader, const char * uniform_name )
{
    int location = glGetUniformLocation( shader, uniform_name );
    if ( location < 0 ) {
        ERROR_LOG( "failed to find uniform '%s' in shader", uniform_name );
    }
    return location;
}

void set_uniform( int uniform, int v )
{
    glUniform1i( uniform, v );
}

void set_uniform( int uniform, float v )
{
    glUniform1f( uniform, v );
}

void set_uniform( int uniform, float ( &v )[ 3 ] )
{
    glUniform3fv( uniform, 1, v );
}

void set_uniform( int uniform, float ( &v )[ 4 ] )
{
    glUniform4fv( uniform, 1, v );
}

void set_uniform( int uniform, vec4 ( &m )[ 4 ] )
{
    glUniformMatrix4fv( uniform, 1, GL_FALSE, (float *) m );
}

void framebuffer_t::init( int width, int height )
{
    unsigned int fbo;
    glGenFramebuffers( 1, &fbo );

    glBindFramebuffer( GL_FRAMEBUFFER, fbo );

    unsigned int local_texture;
    glGenTextures( 1, &local_texture );
    glBindTexture( GL_TEXTURE_2D, local_texture );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        local_texture,
        0
    );

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    id = fbo;
    texture = local_texture;
}
