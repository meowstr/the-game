#include "font.hpp"

#include "logging.hpp"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

struct parse_stream_t {
    const char * data;
    int size;
    int i;

    char getc()
    {
        char c = data[ i ];
        i++;
        return c;
    }

    int eof()
    {
        return i >= size;
    }
};

static int parse_token( parse_stream_t * stream, const char * token )
{
    int i = 0;
    int len = strlen( token );
    while ( !stream->eof() ) {
        if ( i >= len ) return 0;

        char c = stream->getc();

        if ( c == token[ i ] ) {
            i++;
        } else {
            i = 0;
        }
    }

    ERROR_LOG( "failed to parse token (%s)", token );

    return 1;
}

static int parse_number( int * out, parse_stream_t * stream )
{
    int value = 0;

    if ( stream->eof() ) return 1;

    int i = 0;

    while ( !stream->eof() ) {
        char c = stream->getc();

        if ( !isgraph( c ) ) {
            if ( i == 0 ) return 1;
            break;
        }

        if ( !isdigit( c ) ) return 1;

        value *= 10;
        value += c - '0';

        i++;
    }

    *out = value;

    return 0;
}

static int parse_string( char * out, parse_stream_t * stream )
{
    if ( stream->eof() ) return 1;

    char c = stream->getc();

    // expect quote
    if ( c != '"' ) return 1;

    for ( int i = 0; i < FONT_STR_LEN; i++ ) {
        if ( stream->eof() ) return 1;

        char c = stream->getc();

        if ( c == '"' ) {
            out[ i ] = '\0';
            return 0;
        } else {
            out[ i ] = c;
        }
    }

    return 1;
}

static void print_font_info( font_t * font )
{
    INFO_LOG( "font description:" );
    INFO_LOG( "  face:        %s", font->face );
    INFO_LOG( "  file:        %s", font->file );
    INFO_LOG( "  size:        %d", font->size );
    INFO_LOG( "  line height: %d", font->line_height );
    INFO_LOG( "  base:        %d", font->base );
    INFO_LOG( "  scale w:     %d", font->scale_w );
    INFO_LOG( "  scale h:     %d", font->scale_h );
    INFO_LOG( "  char count:  %d", font->char_count );
}

static int parse_font( font_t * font, parse_stream_t * stream )
{
    int error = 0;
    error += parse_token( stream, "info face=" );
    error += parse_string( font->face, stream );
    error += parse_token( stream, "size=" );
    error += parse_number( &font->size, stream );
    error += parse_token( stream, "lineHeight=" );
    error += parse_number( &font->line_height, stream );
    error += parse_token( stream, "base=" );
    error += parse_number( &font->base, stream );
    error += parse_token( stream, "scaleW=" );
    error += parse_number( &font->scale_w, stream );
    error += parse_token( stream, "scaleH=" );
    error += parse_number( &font->scale_h, stream );
    error += parse_token( stream, "file=" );
    error += parse_string( font->file, stream );
    error += parse_token( stream, "chars count=" );
    error += parse_number( &font->char_count, stream );

    if ( error == 0 ) {
        print_font_info( font );
    }

    if ( error == 0 ) {
        for ( int i = 0; i < font->char_count; i++ ) {
            int id;
            error += parse_token( stream, "char id=" );
            error += parse_number( &id, stream );

            if ( id < 0 || id >= FONT_CHAR_COUNT ) continue;

            font_char_t * c = font->chars + id;

            error += parse_token( stream, "x=" );
            error += parse_number( &c->x, stream );
            error += parse_token( stream, "y=" );
            error += parse_number( &c->y, stream );
            error += parse_token( stream, "width=" );
            error += parse_number( &c->width, stream );
            error += parse_token( stream, "height=" );
            error += parse_number( &c->height, stream );
            error += parse_token( stream, "xoffset=" );
            error += parse_number( &c->x_offset, stream );
            error += parse_token( stream, "yoffset=" );
            error += parse_number( &c->y_offset, stream );
            error += parse_token( stream, "xadvance=" );
            error += parse_number( &c->x_advance, stream );

            if ( error ) break;
        }
    }

    return error;
}

/// the best font parser ever written :3
int font_t::init( unsigned char * data, int data_size )
{
    parse_stream_t stream;
    stream.data = (const char *) data;
    stream.size = data_size;
    stream.i = 0;

    int error = parse_font( this, &stream );

    if ( error ) {
        ERROR_LOG( "failed to parse font" );
    }

    return error ? 1 : 0;
}

int font_t::calc_glyphs(
    glyph_t * out,
    int * out_count,
    float * out_width,
    float * out_height,
    const char * text
)
{
    float cursor_x = 0.0f;
    float cursor_y = 0.0f;

    float text_width = 0.0f;
    float text_height = 0.0f;

    const font_char_t * last_char_data = NULL;

    int text_len = strlen( text );

    int count = 0;

    for ( int i = 0; i < text_len; i++ ) {
        char c = text[ i ];

        if ( c == '\n' ) {
            cursor_y += line_height;
            cursor_x = 0.0f;
            continue;
        }

        if ( (int) c < 0 || (int) c >= FONT_CHAR_COUNT ) continue;

        const font_char_t * char_data = chars + c;

        glyph_t glyph;

        glyph.u1 = (float) ( char_data->x ) / scale_w;
        glyph.u2 = (float) ( char_data->x + char_data->width ) / scale_w;
        glyph.v1 = (float) ( char_data->y ) / scale_h;
        glyph.v2 = (float) ( char_data->y + char_data->height ) / scale_h;

        // if ( last_char_data ) {
        //     cursor_x += last_char_data->kernings[ c ];
        // }

        glyph.x = cursor_x + char_data->x_offset;
        glyph.y = cursor_y + char_data->y_offset;
        glyph.w = char_data->width;
        glyph.h = char_data->height;

        out[ count ] = glyph;
        count++;

        cursor_x += char_data->x_advance;

        last_char_data = char_data;

        if ( cursor_x > text_width ) {
            text_width = cursor_x;
        }

        text_height = cursor_y + line_height;
    }

    *out_width = text_width;
    *out_height = text_height;
    *out_count = count;

    return 0;
}
