#version 330 core
layout (location = 0) in vec2 aPos;

uniform vec3 u_proj_r0;
uniform vec3 u_proj_r1;

void main() {
    float x = aPos.x * u_proj_r0.x + u_proj_r0.z;
    float y = aPos.y * u_proj_r1.y + u_proj_r1.z;
    gl_Position = vec4(x, y, 0.0, 1.0);
}version 330 core
layout (location = 0) in vec2 aPos;

uniform vec3 u_proj_r0;
uniform vec3 u_proj_r1;

void main() {
    float x = aPos.x * u_proj_r0.x + u_proj_r0.z;
    float y = aPos.y * u_proj_r1.y + u_proj_r1.z;
    gl_Position = vec4(x, y, 0.0, 1.0);
}
