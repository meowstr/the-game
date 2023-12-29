#include "shape.hpp"

void rect_t::vertices( float * out_data ) const
{
    out_data[ 0 ] = x;
    out_data[ 1 ] = y;
    out_data[ 2 ] = 0.0f;

    out_data[ 3 ] = x + w;
    out_data[ 4 ] = y;
    out_data[ 5 ] = 0.0f;

    out_data[ 6 ] = x;
    out_data[ 7 ] = y + h;
    out_data[ 8 ] = 0.0f;

    out_data[ 9 ] = x + w;
    out_data[ 10 ] = y + h;
    out_data[ 11 ] = 0.0f;

    out_data[ 12 ] = x + w;
    out_data[ 13 ] = y;
    out_data[ 14 ] = 0.0f;

    out_data[ 15 ] = x;
    out_data[ 16 ] = y + h;
    out_data[ 17 ] = 0.0f;
}

void rect_t::margin( float amount )
{
    x += amount;
    y += amount;
    w -= amount * 2;
    h -= amount * 2;
}

float rect_t::center_x()
{
    return x + w * 0.5f;
}

float rect_t::center_y()
{
    return y + h * 0.5f;
}
