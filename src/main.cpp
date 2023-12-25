#include "hardware.hpp"
#include "logging.hpp"

#include <GLES2/gl2.h>

#include <math.h>

typedef struct rgb_color_t {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} rgb_color_t;

typedef struct hsv_color_t {
    unsigned char h;
    unsigned char s;
    unsigned char v;
} hsv_color_t;

rgb_color_t hsv_to_rgb( hsv_color_t hsv )
{
    rgb_color_t rgb;
    unsigned char region, remainder, p, q, t;

    if ( hsv.s == 0 ) {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    region = hsv.h / 43;
    remainder = ( hsv.h - ( region * 43 ) ) * 6;

    p = ( hsv.v * ( 255 - hsv.s ) ) >> 8;
    q = ( hsv.v * ( 255 - ( ( hsv.s * remainder ) >> 8 ) ) ) >> 8;
    t = ( hsv.v * ( 255 - ( ( hsv.s * ( 255 - remainder ) ) >> 8 ) ) ) >> 8;

    switch ( region ) {
    case 0:
        rgb.r = hsv.v;
        rgb.g = t;
        rgb.b = p;
        break;
    case 1:
        rgb.r = q;
        rgb.g = hsv.v;
        rgb.b = p;
        break;
    case 2:
        rgb.r = p;
        rgb.g = hsv.v;
        rgb.b = t;
        break;
    case 3:
        rgb.r = p;
        rgb.g = q;
        rgb.b = hsv.v;
        break;
    case 4:
        rgb.r = t;
        rgb.g = p;
        rgb.b = hsv.v;
        break;
    default:
        rgb.r = hsv.v;
        rgb.g = p;
        rgb.b = q;
        break;
    }

    return rgb;
}

static void loop()
{
    static float time = 0.0f;

    time += 1.0f / 60.0f;

    hsv_color_t hsv{
        (unsigned char) ( ( (int) ( time * 10 ) ) % 255 ),
        255,
        (unsigned char) ( 20 + sin( time * 2.0f ) * 20 ) };
    rgb_color_t color = hsv_to_rgb( hsv );

    glClearColor( color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
}

int main()
{
    INFO_LOG( "meow" );

    hardware_init();

    hardware_set_loop( loop );

    return 0;
}
