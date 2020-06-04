#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform int currentFrame;
uniform int count;
uniform int height;
uniform float displacementRate;
uniform vec3 displacement;
uniform bool reflection;
uniform vec3 reflectionDisplacement;
uniform vec4 reflectionColor;

float rand(float n){return fract(sin(n) * 43758.5453123);}
float noise(float p){
    float fl = floor(p);
    float fc = fract(p);
    return mix(rand(fl), rand(fl + 1.0), fc);
}

void main() {
    for (int i = 0; i < count; i++) {
        float moveX = noise(i) * currentFrame;
        float moveZ = noise(i + 15) * currentFrame;
        bool displace = rand(i) > (1.0 - displacementRate);
        
        vec3 p0 = gl_PositionIn[0].xyz;
        p0.y += i * height;
        if (displace){
            p0.x += displacement.x;
            p0.y += displacement.y;
            p0.z += displacement.z;
        }
        p0.x += moveX;
        p0.z += moveZ;
        gl_Position = gl_ModelViewProjectionMatrix * vec4(p0, 1.0);
        gl_FrontColor = gl_FrontColorIn[0];
        EmitVertex();
        
        vec3 p1 = gl_PositionIn[1].xyz;
        p1.y += i * height;
        if (displace){
            p1.x += displacement.x;
            p1.y += displacement.y;
            p1.z += displacement.z;
        }
        p1.x += moveX;
        p1.z += moveZ;
        gl_Position = gl_ModelViewProjectionMatrix * vec4(p1, 1.0);
        gl_FrontColor = gl_FrontColorIn[1];
        EmitVertex();
        
        EndPrimitive();
        
        if (reflection) {
            p0.x += reflectionDisplacement.x;
            p0.y += reflectionDisplacement.y;
            p0.z += reflectionDisplacement.z;
            gl_Position = gl_ModelViewProjectionMatrix * vec4(p0, 1.0);
            gl_FrontColor = reflectionColor;
//            gl_FrontColor.a = 0.5;
            EmitVertex();

            p1.x += reflectionDisplacement.x;
            p1.y += reflectionDisplacement.y;
            p1.z += reflectionDisplacement.z;
            gl_Position = gl_ModelViewProjectionMatrix * vec4(p1, 1.0);
            gl_FrontColor = reflectionColor;
//            gl_FrontColor.a = 0.5;
            EmitVertex();
            EndPrimitive();
        }
        
        
    }
}

