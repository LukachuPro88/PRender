#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform vec4 u_color;

void main() {
    vec2 uv = TexCoord * 2.0 - 1.0;

    if (dot(uv, uv) > 1.0) {
        discard;
    }

    FragColor = u_color;
}
