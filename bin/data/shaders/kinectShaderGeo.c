#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform float length;

void main() {
    vec3 p0 = gl_PositionIn[0].xyz;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(p0, 1.0);
    gl_FrontColor = gl_FrontColorIn[0];
    EmitVertex();

    p0.x += length;
    p0.y += length;
    p0.z += length;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(p0, 1.0);
    gl_FrontColor = gl_FrontColorIn[0];
    EmitVertex();


}

