#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec3 u_proj_r0;
uniform vec3 u_proj_r1;
uniform vec2 u_position;
uniform vec2 u_size;

void main() {
    TexCoord = aTexCoord;
    vec2 worldPos = u_position + (aPos * u_size);
    float x = dot(vec3(worldPos, 1.0), u_proj_r0);
    float y = dot(vec3(worldPos, 1.0), u_proj_r1);
    gl_Position = vec4(x, y, 0.0, 1.0);
}
