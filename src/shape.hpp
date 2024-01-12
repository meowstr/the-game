#pragma once

struct rect_t {
    float x;
    float y;
    float w;
    float h;

    /// 6 vec3 vertices
    /// out_data should be at least 18 floats long
    void vertices( float * out_data ) const;

    /// 6 vec2 vertices
    /// out_data should be at least 12 floats long
    void vertices_2d( float * out_data ) const;

    void margin( float amount );

    float center_x();
    float center_y();
};
