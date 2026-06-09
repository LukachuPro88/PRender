#define STB_IMAGE_IMPLEMENTATION
#include "prender.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static PR_Shader quad_shader = {0};
static PR_Shader circle_shader = {0};
static PR_Shader sprite_shader = {0};

static unsigned int quad_vao = 0;
static unsigned int quad_vbo = 0;

static unsigned int shape_vao = 0;
static unsigned int shape_vbo = 0;
static size_t shape_vbo_capacity = 0;

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
        fprintf(stderr,
                "[PRender GLSL Error] Vertex compile failure (%s):\n%s\n",
                vertPath, infoLog);
    }

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, (const char **)&fragSource, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        fprintf(stderr,
                "[PRender GLSL Error] Fragment compile failure (%s):\n%s\n",
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
    quad_shader = PR_LoadShader("shaders/quad.vert", "shaders/quad.frag");
    if (quad_shader.id == 0)
        return false;

    circle_shader = PR_LoadShader("shaders/circle.vert", "shaders/circle.frag");
    if (circle_shader.id == 0)
        return false;

    sprite_shader = PR_LoadShader("shaders/sprite.vert", "shaders/sprite.frag");
    if (sprite_shader.id == 0)
        return false;

    float vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    };

    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);

    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));

    glGenVertexArrays(1, &shape_vao);
    glGenBuffers(1, &shape_vbo);

    glBindVertexArray(shape_vao);
    glBindBuffer(GL_ARRAY_BUFFER, shape_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(PM_Vec2), (void *)0);

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
    glUniform4f(glGetUniformLocation(quad_shader.id, "u_color"), color.r,
                color.g, color.b, color.a);

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

    glUniform2f(glGetUniformLocation(circle_shader.id, "u_position"),
                position.x, position.y);
    glUniform2f(glGetUniformLocation(circle_shader.id, "u_size"), radius * 2.0f,
                radius * 2.0f);
    glUniform4f(glGetUniformLocation(circle_shader.id, "u_color"), color.r,
                color.g, color.b, color.a);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PR_DrawShape2D(PR_Window *window, const PM_Vec2 *vertices,
                    size_t vertex_count, PR_Color color) {
    if (!window || quad_shader.id == 0 || !vertices || vertex_count < 3)
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

    glUniform2f(glGetUniformLocation(quad_shader.id, "u_position"), 0.0f, 0.0f);
    glUniform2f(glGetUniformLocation(quad_shader.id, "u_size"), 1.0f, 1.0f);
    glUniform4f(glGetUniformLocation(quad_shader.id, "u_color"), color.r,
                color.g, color.b, color.a);

    glBindBuffer(GL_ARRAY_BUFFER, shape_vbo);
    size_t required_size = vertex_count * sizeof(PM_Vec2);

    if (required_size > shape_vbo_capacity) {
        glBufferData(GL_ARRAY_BUFFER, required_size, vertices, GL_DYNAMIC_DRAW);
        shape_vbo_capacity = required_size;
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, required_size, vertices);
    }

    glBindVertexArray(shape_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)vertex_count);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

PR_Texture PR_LoadTexture(const char *filepath) {
    PR_Texture texture = {0};
    int channels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data =
        stbi_load(filepath, &texture.width, &texture.height, &channels, 4);
    if (!data) {
        fprintf(stderr, "[ERROR]: Failed to load texture: %s\n", filepath);
        return texture;
    }

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.width, texture.height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

void PR_DeleteTexture(PR_Texture texture) {
    if (texture.id) {
        glDeleteTextures(1, &texture.id);
    }
}

void PR_DrawSprite(PR_Window *window, PR_Texture texture, PM_Vec2 position,
                   PM_Vec2 size, PR_Color color) {
    if (!window || sprite_shader.id == 0 || texture.id == 0)
        return;

    PR_UseShader(sprite_shader);

    float width = (float)window->width;
    float height = (float)window->height;

    PM_Vec3 proj_r0 = {2.0f / width, 0.0f, -1.0f};
    PM_Vec3 proj_r1 = {0.0f, -2.0f / height, 1.0f};

    glUniform3f(glGetUniformLocation(sprite_shader.id, "u_proj_r0"), proj_r0.x,
                proj_r0.y, proj_r0.z);
    glUniform3f(glGetUniformLocation(sprite_shader.id, "u_proj_r1"), proj_r1.x,
                proj_r1.y, proj_r1.z);

    glUniform2f(glGetUniformLocation(sprite_shader.id, "u_position"),
                position.x, position.y);
    glUniform2f(glGetUniformLocation(sprite_shader.id, "u_size"), size.x,
                size.y);
    glUniform4f(glGetUniformLocation(sprite_shader.id, "u_color"), color.r,
                color.g, color.b, color.a);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glUniform1i(glGetUniformLocation(sprite_shader.id, "u_texture"), 0);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PR_CleanRenderer(void) {
    if (quad_vao)
        glDeleteVertexArrays(1, &quad_vao);
    if (quad_vbo)
        glDeleteBuffers(1, &quad_vbo);

    if (shape_vao)
        glDeleteVertexArrays(1, &shape_vao);
    if (shape_vbo)
        glDeleteBuffers(1, &shape_vbo);

    PR_DeleteShader(quad_shader);
    PR_DeleteShader(circle_shader);
    PR_DeleteShader(sprite_shader);
}
