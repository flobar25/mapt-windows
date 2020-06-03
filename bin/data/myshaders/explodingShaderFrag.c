#version 120
#extension GL_ARB_texture_rectangle: enable
#extension GL_EXT_gpu_shader4: enable

uniform float time;
uniform float length = 100;

void main() {
    gl_FragColor = gl_Color;
    gl_FragColor.a -= time/length;
}
