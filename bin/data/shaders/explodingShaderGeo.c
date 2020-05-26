#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform float time;

vec3 GetNormal()
{
   vec3 a = vec3(gl_PositionIn[0].xyz) - vec3(gl_PositionIn[1].xyz);
   vec3 b = vec3(gl_PositionIn[2].xyz) - vec3(gl_PositionIn[1].xyz);
   return normalize(cross(a, b));
}

vec3 explode(vec3 position, vec3 normal)
{
    float magnitude = 2.0;
//    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
    vec3 direction = normal * time * magnitude;
    return position + direction;
//    return gl_ModelViewProjectionMatrix * vec4(gl_PositionIn[0].xyz, 1.0);
} 




void main() {
    vec3 normal = GetNormal();

    gl_Position = gl_ModelViewProjectionMatrix * vec4(explode(gl_PositionIn[0].xyz, normal), 1.0);
    gl_FrontColor = gl_FrontColorIn[0];
    EmitVertex();
    gl_Position =  gl_ModelViewProjectionMatrix * vec4(explode(gl_PositionIn[1].xyz, normal), 1.0);
    gl_FrontColor = gl_FrontColorIn[1];
    EmitVertex();
    gl_Position =  gl_ModelViewProjectionMatrix * vec4(explode(gl_PositionIn[2].xyz, normal), 1.0);
    gl_FrontColor = gl_FrontColorIn[2];
    EmitVertex();
    EndPrimitive();

} 

