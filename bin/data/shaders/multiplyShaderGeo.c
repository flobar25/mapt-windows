#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform int count;
uniform int height;

void main() {
    for (int i = 0; i < count; i++) {
//        for (int j = 0; i < 4; j++){
            vec3 p0 = gl_PositionIn[0].xyz;
            p0.y += i * height;
            gl_Position = gl_ModelViewProjectionMatrix * vec4(p0, 1.0);
            gl_FrontColor = gl_FrontColorIn[0];
            EmitVertex();
        
            p0 = gl_PositionIn[1].xyz;
            p0.y += i * height;
            gl_Position = gl_ModelViewProjectionMatrix * vec4(p0, 1.0);
            gl_FrontColor = gl_FrontColorIn[1];
            EmitVertex();
//        }
        
        EndPrimitive();
    }
}

