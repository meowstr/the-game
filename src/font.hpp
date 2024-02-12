#pragma once

#define FONT_STR_LEN    64
#define FONT_CHAR_COUNT 128

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int x_offset;
    int y_offset;
    int x_advance;
} font_char_t;

typedef struct {
    float u1;
    float u2;
    float v1;
    float v2;

    float x;
    float y;
    float w;
    float h;

} glyph_t;

struct font_t {
    char face[ FONT_STR_LEN ];
    char file[ FONT_STR_LEN ];

    font_char_t chars[ FONT_CHAR_COUNT ];

    int size;
    int line_height;
    int base;
    int scale_w;
    int scale_h;
    int char_count;

    int init( unsigned char * data, int size );

    int calc_glyphs(
        glyph_t * out,
        int * out_count,
        float * out_width,
        float * out_height,
        const char * text
    );
};

// void destroy_font( font_t * font );
