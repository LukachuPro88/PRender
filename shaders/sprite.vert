#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform vec3 u_proj_r0;
uniform vec3 u_proj_r1;
uniform vec2 u_position;
uniform vec2 u_size;

void main() {
    vec2 world_pos = aPos * u_size + u_position;
    float x = world_pos.x * u_proj_r0.x + u_proj_r0.z;
    float y = world_pos.y * u_proj_r1.y + u_proj_r1.z;

    gl_Position = vec4(x, y, 0.0, 1.0);
    TexCoords = aTexCoords;
}
