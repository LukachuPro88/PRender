#include "prender.h"
#include <glad/glad.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static PR_Shader quad_shader = {0};
static PR_Shader circle_shader = {0};

static unsigned int quad_vao = 0;
static unsigned int quad_vbo = 0;

char *PR_ReadShaderFile(const char *filepath) {
  FILE *file = fopen(filepath, "rb");
  if (!file) {
    fprintf(stderr, "[ERROR]: Failed to open shader: %s\n", filepath);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = (char *)malloc(length + 1);
  if (!buffer) {
    fprintf(stderr,
            "[PRender Error] Out of memory allocating shader buffer.\n");
    fclose(file);
    return NULL;
  }

  size_t bytesRead = fread(buffer, 1, length, file);
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}

PR_Shader PR_LoadShader(const char *vertPath, const char *fragPath) {
  PR_Shader program = {0};
  int success;
  char infoLog[512];

  char *vertSource = PR_ReadShaderFile(vertPath);
  char *fragSource = PR_ReadShaderFile(fragPath);
  if (!vertSource || !fragSource)
    return program;

  unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, (const char **)&vertSource, NULL);
  glCompileShader(vertex);

  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    fprintf(stderr, "[PRender GLSL Error] Vertex compile failure (%s):\n%s\n",
            vertPath, infoLog);
  }

  unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, (const char **)&fragSource, NULL);
  glCompileShader(fragment);

  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    fprintf(stderr, "[PRender GLSL Error] Fragment compile failure (%s):\n%s\n",
            fragPath, infoLog);
  }

  program.id = glCreateProgram();
  glAttachShader(program.id, vertex);
  glAttachShader(program.id, fragment);
  glLinkProgram(program.id);

  glGetProgramiv(program.id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program.id, 512, NULL, infoLog);
    fprintf(stderr, "[PRender GLSL Link Error]:\n%s\n", infoLog);
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
  free(vertSource);
  free(fragSource);

  return program;
}

void PR_UseShader(PR_Shader shader) { glUseProgram(shader.id); }

void PR_DeleteShader(PR_Shader shader) { glDeleteProgram(shader.id); }

bool PR_InitRenderer(void) {
  quad_shader = PR_LoadShader("src/shaders/quad.vert", "src/shaders/quad.frag");
  if (quad_shader.id == 0)
    return false;

  circle_shader =
      PR_LoadShader("src/shaders/circle.vert", "src/shaders/circle.frag");
  if (circle_shader.id == 0)
    return false;

  float vertices[] = {
      0.0f, 1.0f, 0.0f, 1.0f, // TOP LEFT
      0.0f, 0.0f, 0.0f, 0.0f, // BOTTOM LEFT
      1.0f, 0.0f, 1.0f, 0.0f, // BOTTOM RIGHT

      0.0f, 1.0f, 0.0f, 1.0f, // TOP LEFT
      1.0f, 0.0f, 1.0f, 0.0f, // BOTTOM RIGHT
      1.0f, 1.0f, 1.0f, 1.0f, // TOP RIGHT
  };

  glGenVertexArrays(1, &quad_vao);
  glGenBuffers(1, &quad_vbo);

  glBindVertexArray(quad_vao);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return true;
}

void PR_DrawQuad(PR_Window *window, PM_Vec2 position, PM_Vec2 size,
                 PR_Color color) {
  if (!window || quad_shader.id == 0)
    return;

  PR_UseShader(quad_shader);

  float width = (float)window->width;
  float height = (float)window->height;

  PM_Vec3 proj_r0 = {2.0f / width, 0.0f, -1.0f};
  PM_Vec3 proj_r1 = {0.0f, -2.0f / height, 1.0f};

  glUniform3f(glGetUniformLocation(quad_shader.id, "u_proj_r0"), proj_r0.x,
              proj_r0.y, proj_r0.z);
  glUniform3f(glGetUniformLocation(quad_shader.id, "u_proj_r1"), proj_r1.x,
              proj_r1.y, proj_r1.z);

  glUniform2f(glGetUniformLocation(quad_shader.id, "u_position"), position.x,
              position.y);
  glUniform2f(glGetUniformLocation(quad_shader.id, "u_size"), size.x, size.y);
  glUniform4f(glGetUniformLocation(quad_shader.id, "u_color"), color.r, color.g,
              color.b, color.a);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void PR_DrawCircle(PR_Window *window, PM_Vec2 position, float radius,
                   PR_Color color) {
  if (!window || circle_shader.id == 0)
    return;

  PR_UseShader(circle_shader);

  float width = (float)window->width;
  float height = (float)window->height;

  PM_Vec3 proj_r0 = {2.0f / width, 0.0f, -1.0f};
  PM_Vec3 proj_r1 = {0.0f, -2.0f / height, 1.0f};

  glUniform3f(glGetUniformLocation(circle_shader.id, "u_proj_r0"), proj_r0.x,
              proj_r0.y, proj_r0.z);
  glUniform3f(glGetUniformLocation(circle_shader.id, "u_proj_r1"), proj_r1.x,
              proj_r1.y, proj_r1.z);

  glUniform2f(glGetUniformLocation(circle_shader.id, "u_position"), position.x,
              position.y);
  glUniform2f(glGetUniformLocation(circle_shader.id, "u_size"), radius * 2.0f,
              radius * 2.0f);
  glUniform4f(glGetUniformLocation(circle_shader.id, "u_color"), color.r,
              color.g, color.b, color.a);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void PR_CleanRenderer(void) {
  if (quad_vao)
    glDeleteVertexArrays(1, &quad_vao);
  if (quad_vbo)
    glDeleteBuffers(1, &quad_vbo);

  PR_DeleteShader(quad_shader);
  PR_DeleteShader(circle_shader);
}
