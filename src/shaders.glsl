#shader shader1_vertex

#version 100
attribute vec3 a_pos;
uniform mat4 u_proj;
uniform mat4 u_model;
void main()
{
    vec4 pos = vec4( a_pos, 1.0 );
    gl_Position = u_proj * u_model * pos;
}

#shader shader1_fragment

#version 100
precision lowp float;
uniform vec4 u_color;
void main()
{
    gl_FragColor = u_color;
}

#shader shader2_vertex

#version 100
attribute vec2 a_pos;
attribute vec2 a_uv;
varying vec2 v_uv;
void main()
{
    v_uv = a_uv;
    gl_Position = vec4( a_pos.x, a_pos.y, 0.0, 1.0 );
}

#shader shader2_fragment

#version 100
precision lowp float;
uniform sampler2D u_texture;
uniform float u_amount;
varying vec2 v_uv;
void main()
{
    vec2 d = vec2( 0.013, 0.013 ) * u_amount;
    vec3 c;
    c.r = texture2D( u_texture, v_uv + d ).r;
    c.g = texture2D( u_texture, v_uv ).g;
    c.b = texture2D( u_texture, v_uv - d ).b;
    gl_FragColor = vec4( c, 1.0 );
}

#shader shader3_vertex

#version 100
attribute vec2 a_pos;
attribute vec2 a_uv;
varying vec2 v_uv;
void main()
{
    v_uv = a_uv;
    gl_Position = vec4( a_pos.x, a_pos.y, 0.0, 1.0 );
}

#shader shader3_fragment

#version 100
precision lowp float;
uniform sampler2d u_texture;
uniform float u_amount;
varying vec2 v_uv;
void main()
{
    vec2 d = vec2( 0.003, 0.003 ) * u_amount;
    vec3 c;
    c.r = texture2d( u_texture, v_uv + d ).r;
    c.g = texture2d( u_texture, v_uv ).g;
    c.b = texture2d( u_texture, v_uv - d ).b;
    gl_fragcolor = vec4( c, 1.0 );
}
