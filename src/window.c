// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "prender.h"
#include <stdio.h>
#include <stdlib.h>

float PR_DeltaTime = 0.0f;

static double last_frame_time = 0.0f;

PR_Window *PR_InitWindow(const unsigned int width, const unsigned int height,
                         const char *title, const PM_Vec2 position, int flags) {
    if (flags == 0) {
        flags = PR_WINDOWED;
    }

    if (!glfwInit()) {
        return NULL;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *handle = NULL;
    GLFWmonitor *monitor = NULL;
    unsigned int final_width = width;
    unsigned int final_height = height;

    if (flags == PR_FULLSCREEN) {
        monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            final_width = mode->width;
            final_height = mode->height;
            handle = glfwCreateWindow(final_width, final_height, title, monitor,
                                      NULL);
        }
    }

    if (!handle) {
        handle = glfwCreateWindow(final_width, final_height, title, NULL, NULL);
    }

    if (!handle) {
        glfwTerminate();
        return NULL;
    }

    if (flags != PR_FULLSCREEN) {
        glfwSetWindowPos(handle, (int)position.x, (int)position.y);
    }

    glfwMakeContextCurrent(handle);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(handle);
        glfwTerminate();
        return NULL;
    }

    PR_Window *window = (PR_Window *)malloc(sizeof(PR_Window));
    if (!window) {
        glfwDestroyWindow(handle);
        glfwTerminate();
        return NULL;
    }

    window->handle = handle;
    window->width = final_width;
    window->height = final_height;
    window->title = title;
    window->position = position;
    window->is_fullscreen = (flags == PR_FULLSCREEN);

    if (flags != PR_FULLSCREEN) {
        window->windowed_width = final_width;
        window->windowed_height = final_height;
        window->windowed_position = position;
    } else {
        window->windowed_width = 800;
        window->windowed_height = 600;
        window->windowed_position = (PM_Vec2){100.0f, 100.0f};
    }

    return window;
}

void PR_SetFullscreen(PR_Window *window, bool fullscreen) {
    if (!window || !window->handle || window->is_fullscreen == fullscreen) {
        return;
    }

    GLFWwindow *handle = (GLFWwindow *)window->handle;

    if (fullscreen) {
        int wx, wy;
        glfwGetWindowPos(handle, &wx, &wy);
        window->windowed_position.x = (float)wx;
        window->windowed_position.y = (float)wy;

        int ww, wh;
        glfwGetWindowSize(handle, &ww, &wh);
        window->windowed_width = (unsigned int)ww;
        window->windowed_height = (unsigned int)wh;

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(handle, monitor, 0, 0, mode->width,
                                 mode->height, mode->refreshRate);
            window->width = (unsigned int)mode->width;
            window->height = (unsigned int)mode->height;
        }
    } else {
        glfwSetWindowMonitor(handle, NULL, (int)window->windowed_position.x,
                             (int)window->windowed_position.y,
                             (int)window->windowed_width,
                             (int)window->windowed_height, 0);
        window->width = window->windowed_width;
        window->height = window->windowed_height;
    }

    window->is_fullscreen = fullscreen;
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
        fprintf(stderr, "[ERROR]: Shader Program Linking Failed:\n%s\n",
                info_log);
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
