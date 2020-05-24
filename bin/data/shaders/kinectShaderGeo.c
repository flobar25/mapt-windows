#version 120
#extension GL_EXT_geometry_shader4 : enable

void main() {
    vec4 p0 = gl_PositionIn[0];
    gl_Position = p0;
    gl_FrontColor = gl_FrontColorIn[0];
    EmitVertex();

    p0.x += 20;
    p0.y += 20;
    p0.z += 20;
    gl_Position = p0;
    gl_FrontColor = gl_FrontColorIn[0];
    EmitVertex();


}

