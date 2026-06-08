#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

void main() {
  vec2 centerRelative = TexCoord - vec2(0.5, 0.5);
  float distance = length(centerRelative);

  if (distance > 0.5) {
    discard;
  }

  FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
