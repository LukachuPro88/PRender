#include "prender.h"
#include <glad/glad.h>
#include <stdbool.h>

static unsigned int global_shader_program = 0;
static unsigned int quad_vao = 0;
static unsigned int quad_vbo = 0;

static const char *default_vert =
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "uniform vec3 u_proj_r0;\n"
    "uniform vec3 u_proj_r1;\n"
    "uniform vec2 u_position;\n"
    "uniform vec2 u_size;\n"
    "void main() {\n"
    "   vec2 world_pos = u_position + (aPos * u_size);\n"
    "   float x = dot(u_proj_r0, vec3(world_pos, 1.0));\n"
    "   float y = dot(u_proj_r1, vec3(world_pos, 1.0));\n"
    "   gl_Position = vec4(x, y, 0.0, 1.0);\n"
    "}\n";

static const char *default_frag = "#version 330 core\n"
                                  "out vec4 FragColor;\n"
                                  "uniform vec4 u_color;\n"
                                  "void main() {\n"
                                  "   FragColor = u_color;\n"
                                  "}\n";

static unsigned int circle_shader_program = 0;

static const char *circle_vert =
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "out vec2 v_uv;\n"
    "uniform vec3 u_proj_r0;\n"
    "uniform vec3 u_proj_r1;\n"
    "uniform vec2 u_position;\n" // This will now be the TRUE center!
    "uniform float u_radius;\n"
    "void main() {\n"
    "   v_uv = aPos;\n"
    "   \n"
    "   // Shift the 0.0 to 1.0 vertex coordinate space by -0.5 \n"
    "   // so that (0,0) sits exactly in the center of the geometry\n"
    "   vec2 centered_offset = aPos - vec2(0.5);\n"
    "   vec2 world_pos = u_position + (centered_offset * (u_radius * 2.0));\n"
    "   \n"
    "   float x = dot(u_proj_r0, vec3(world_pos, 1.0));\n"
    "   float y = dot(u_proj_r1, vec3(world_pos, 1.0));\n"
    "   gl_Position = vec4(x, y, 0.0, 1.0);\n"
    "}\n";

static const char *circle_frag =
    "#version 330 core\n"
    "in vec2 v_uv;\n"
    "out vec4 FragColor;\n"
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "   // Normalize local space relative to shape center\n"
    "   vec2 local_pos = (v_uv * 2.0) - vec2(1.0);\n"
    "   float distance = length(local_pos);\n"
    "   \n"
    "   float fade = fwidth(distance);\n"
    "   float alpha = 1.0 - smoothstep(1.0 - fade, 1.0, distance);\n"
    "   \n"
    "   if (distance > 1.0) discard;\n"
    "   \n"
    "   FragColor = vec4(u_color.rgb, u_color.a * alpha);\n"
    "}\n";

bool PR_InitRenderer(void) {
  global_shader_program = PR_CreateShader(default_vert, default_frag);
  if (global_shader_program == 0)
    return false;

  // FIXED: Compile the circle program too!
  circle_shader_program = PR_CreateShader(circle_vert, circle_frag);
  if (circle_shader_program == 0)
    return false;

  float vertices[] = {
      0.0f, 1.0f, // TOP LEFT
      0.0f, 0.0f, // BOTTOM LEFT
      1.0f, 0.0f, // BOTTOM RIGHT

      0.0f, 1.0f, // TOP LEFT
      1.0f, 0.0f, // BOTTOM RIGHT
      1.0f, 1.0f, // TOP RIGHT
  };

  glGenVertexArrays(1, &quad_vao);
  glGenBuffers(1, &quad_vbo);

  glBindVertexArray(quad_vao);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return true;
}

void PR_DrawQuad(PR_Window *window, PM_Vec2 position, PM_Vec2 size,
                 PR_Color color) {
  if (!window)
    return;

  glUseProgram(global_shader_program);

  float width = (float)window->width;
  float height = (float)window->height;

  PM_Vec3 proj_r0 = {2.0f / width, 0.0f, -1.0f};
  PM_Vec3 proj_r1 = {0.0f, -2.0f / height, 1.0f};

  glUniform3f(glGetUniformLocation(global_shader_program, "u_proj_r0"),
              proj_r0.x, proj_r0.y, proj_r0.z);
  glUniform3f(glGetUniformLocation(global_shader_program, "u_proj_r1"),
              proj_r1.x, proj_r1.y, proj_r1.z);

  glUniform2f(glGetUniformLocation(global_shader_program, "u_position"),
              position.x, position.y);
  glUniform2f(glGetUniformLocation(global_shader_program, "u_size"), size.x,
              size.y);
  glUniform4f(glGetUniformLocation(global_shader_program, "u_color"), color.r,
              color.g, color.b, color.a);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void PR_DrawCircle(PR_Window *window, PM_Vec2 position, float radius,
                   PR_Color color) {
  if (!window)
    return;

  glUseProgram(circle_shader_program);

  float width = (float)window->width;
  float height = (float)window->height;

  // FIXED: Match the exact projection row layout used in PR_DrawQuad
  PM_Vec3 proj_r0 = {2.0f / width, 0.0f, -1.0f};
  PM_Vec3 proj_r1 = {0.0f, -2.0f / height, 1.0f};

  glUniform3f(glGetUniformLocation(circle_shader_program, "u_proj_r0"),
              proj_r0.x, proj_r0.y, proj_r0.z);
  glUniform3f(glGetUniformLocation(circle_shader_program, "u_proj_r1"),
              proj_r1.x, proj_r1.y, proj_r1.z);

  glUniform2f(glGetUniformLocation(circle_shader_program, "u_position"),
              position.x, position.y);
  glUniform1f(glGetUniformLocation(circle_shader_program, "u_radius"), radius);
  glUniform4f(glGetUniformLocation(circle_shader_program, "u_color"), color.r,
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
  if (global_shader_program)
    glDeleteProgram(global_shader_program);
  if (circle_shader_program)
    glDeleteProgram(circle_shader_program);
}
