// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "prender.h"
#include <stdio.h>
#include <stdlib.h>

// Declare delta time
float PR_DeltaTime = 0.0f;

// Last frame time to calculate delta time
static double last_frame_time = 0.0f;

PR_Window *PR_InitWindow(const unsigned width, const unsigned height,
                         const char *title, const PM_Vec2 position) {
  if (!glfwInit()) {
    fprintf(stderr, "[ERROR]: Failed to initialize GLFW\n");
    return NULL;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  // Allocate the wrapper struct on the heap so it persists after the function
  // returns
  PR_Window *window = (PR_Window *)malloc(sizeof(PR_Window));
  if (!window) {
    fprintf(stderr,
            "[ERROR]: Failed to allocate memory for PR_Window wrapper\n");
    glfwTerminate();
    return NULL;
  }

  window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window->handle) {
    fprintf(stderr, "[ERROR]: Failed to create GLFW window\n");
    free(window);
    glfwTerminate();
    return NULL;
  }

  window->width = width;
  window->height = height;
  window->title = title;
  window->position = position;

  glfwSetWindowPos(window->handle, (int)position.x, (int)position.y);
  glfwShowWindow(window->handle);
  glfwMakeContextCurrent(window->handle);

  if (!gladLoadGL()) {
    fprintf(stderr, "[ERROR]: Failed to initialize OpenGL context / GLAD\n");
    glfwDestroyWindow(window->handle);
    free(window);
    glfwTerminate();
    return NULL;
  }

  glViewport(0, 0, width, height);

  return window;
}

void PR_KillWindow(PR_Window *window) {
  if (window) {
    if (window->handle) {
      glfwDestroyWindow(window->handle);
    }
    free(window);
  }
  glfwTerminate();
}

bool PR_WindowShouldClose(PR_Window *window) {
  if (!window || !window->handle)
    return true;
  return glfwWindowShouldClose(window->handle);
}

void PR_StartFrame(PR_Color color) {
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void PR_EndFrame(PR_Window *window) {
  if (window && window->handle) {
    glfwSwapBuffers(window->handle);
  }

  // Calculate delta time
  double current_time = glfwGetTime();
  PR_DeltaTime = (float)(current_time - last_frame_time);
  last_frame_time = current_time;

  glfwPollEvents();
}

unsigned int PR_CreateShader(const char *vertex_source,
                             const char *fragment_source) {
  int success;
  char info_log[512];

  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source, NULL);
  glCompileShader(vertex_shader);

  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    fprintf(stderr, "[ERROR]: Vertex Shader Compilation Failed:\n%s\n",
            info_log);
    return 0;
  }

  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source, NULL);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    fprintf(stderr, "[ERROR]: Fragment Shader Compilation Failed:\n%s\n",
            info_log);
    glDeleteShader(vertex_shader);
    return 0;
  }

  unsigned int program_id = glCreateProgram();
  glAttachShader(program_id, vertex_shader);
  glAttachShader(program_id, fragment_shader);
  glLinkProgram(program_id);

  glGetProgramiv(program_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program_id, 512, NULL, info_log);
    fprintf(stderr, "[ERROR]: Shader Program Linking Failed:\n%s\n", info_log);
    program_id = 0;
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return program_id;
}

void PR_LockWindowSize(PR_Window *window) {
  if (!window || !window->handle)
    return;
  glfwSetWindowSizeLimits(window->handle, window->width, window->height,
                          window->width, window->height);
}
