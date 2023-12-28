#pragma once

struct rect_t {
    float x;
    float y;
    float w;
    float h;

    /// 6 vec3 vertices
    /// out_data should be at least 18 floats long
    void vertices( float * out_data ) const;

    void margin( float amount );
};
