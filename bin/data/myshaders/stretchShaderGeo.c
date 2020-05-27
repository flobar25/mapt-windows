#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform vec3 targetPosition;
uniform int time;
uniform int stretchFramesCount;

void main() {
    vec3 p0 = gl_PositionIn[0].xyz;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(p0, 1.0);
    gl_FrontColor = gl_FrontColorIn[0];
    EmitVertex();
    
    if (time < stretchFramesCount) {
        // going towards the target position
        vec3 currentPosition = (targetPosition / stretchFramesCount) * time;
        p0 += currentPosition;
        gl_Position = gl_ModelViewProjectionMatrix * vec4(p0, 1.0);
        gl_FrontColor = gl_FrontColorIn[0];
        EmitVertex();
    } else if (time < stretchFramesCount * 2) {
        // going back to the start position
        vec3 currentPosition = targetPosition - ((targetPosition / stretchFramesCount) * (time - stretchFramesCount));
        p0 += currentPosition;
        gl_Position = gl_ModelViewProjectionMatrix * vec4(p0, 1.0);
        gl_FrontColor = gl_FrontColorIn[0];
        EmitVertex();
    }

}

