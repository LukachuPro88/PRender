#ifndef PRENDER_H
#define PRENDER_H

#include <pmath.h>
#include <stdbool.h>

typedef struct {
  float r;
  float g;
  float b;
  float a;
} PR_Color;

#define PR_BLACK (PR_Color){0.00f, 0.00f, 0.00f, 1.00f}
#define PR_WHITE (PR_Color){1.00f, 1.00f, 1.00f, 1.00f}
#define PR_BLUE (PR_Color){0.12f, 0.35f, 0.67f, 1.00f}      // Slate/Royal Blue
#define PR_DARK_GRAY (PR_Color){0.10f, 0.10f, 0.10f, 1.00f} // Nice dark canvas
#define PR_CHARCOAL (PR_Color){0.15f, 0.15f, 0.18f, 1.00f}  // Modern background

typedef struct {
  unsigned int id;
} PR_Shader;

/**
 * @brief Reads the entire contents of a shader file into a dynamically
 * allocated string.
 *
 * @param *filepath The path to the shader file on disk.
 *
 * @return A null-terminated string containing the file contents, or NULL if the
 * file fails to open.
 *
 * @note The caller is responsible for freeing the returned memory buffer.
 */
char *PR_ReadShaderFile(const char *filepath);

/**
 * @brief Loads, compiles, and links a vertex and fragment shader into a
 * complete OpenGL shader program.
 *
 * @param *vertPath The path to the vertex shader file (.vert).
 * @param *fragPath The path to the fragment shader file (.frag).
 *
 * @return A PR_Shader struct containing the linked program ID, or an ID of 0 if
 * compilation or linking fails.
 */
PR_Shader PR_LoadShader(const char *vertPath, const char *fragPath);

/**
 * @brief Activates the specified shader program for subsequent rendering draw
 * calls.
 *
 * @param shader The PR_Shader program target to activate.
 */
void PR_UseShader(PR_Shader shader);

/**
 * @brief Deletes the specified shader program from GPU memory.
 *
 * @param shader The PR_Shader program target to destroy.
 */
void PR_DeleteShader(PR_Shader shader);
// Global deltatime
extern float PR_DeltaTime;

typedef struct GLFWwindow GLFWwindow;

typedef struct {
  GLFWwindow *handle;
  unsigned int width;
  unsigned int height;
  const char *title;
  PM_Vec2 position;
} PR_Window;

/**
 * @brief Initializes and creates a PR_Window.
 *
 * @note To include a position use pmath.h PM_Vec2.
 *
 * @param width The width for the window.
 * @param height The height for the window.
 * @param *title The title for the window.
 * @param position The windows position as PM_Vec2.
 *
 * @return The created window
 */
PR_Window *PR_InitWindow(const unsigned width, const unsigned height,
                         const char *title, const PM_Vec2 position);

/**
 * @brief Kills the given window.
 *
 * @param *window The window to kill.
 */
void PR_KillWindow(PR_Window *window);

/**
 * @brief Checks if the window should close.
 *
 * @param *window The window to check.
 *
 * @return True if window should close.
 */
bool PR_WindowShouldClose(PR_Window *window);

/**
 * @brief Prepares the window for a new frame by clearing the back buffer.
 *
 * @param color The color to render the frame
 */
void PR_StartFrame(PR_Color color);

/**
 * @brief Swaps the front and back buffers and polls window events.
 * @param *window The window to update.
 */
void PR_EndFrame(PR_Window *window);

/**
 * @brief Compiles a vertex and fragment shader, then links them into a shader
 * program.
 *
 * @param *vertex_source The raw GLSL source string for the vertex shader.
 * @param *fragment_source The raw GLSL source string for the fragment shader.
 *
 * @return The generated OpenGL program ID, or 0 if compilation/linking failed.
 */
unsigned int PR_CreateShader(const char *vertex_source,
                             const char *fragment_source);

/**
 * @brief Initializes the internal global rendering engine state, buffers, and
 * shaders.
 *
 * @return True if the graphics pipeline was successfully prepared.
 */
bool PR_InitRenderer(void);

/**
 * @brief Cleans up and deletes all global rendering engine GPU resources.
 */
void PR_CleanRenderer(void);

/**
 * @brief Draws a filled rectangle onto the active window canvas.
 *
 * @param *window The window context to get dimensions from.
 * @param position The position layout vector for the top-left coordinate.
 * @param size The structural width and height dimensions of the rectangle.
 * @param color The PR_Color used to fill the rectangle.
 */
void PR_DrawQuad(PR_Window *window, PM_Vec2 position, PM_Vec2 size,
                 PR_Color color);

/**
 * @brief Draws a filled circle onto the active canvas.
 *
 * @param *window The window context to get dimensions from.
 * @param position THe position layout vector for the top-left coordinate.
 * @param radius The structural radius dimension of the circle.
 * @param color The PR_Color used to fill the rectangle.
 */
void PR_DrawCircle(PR_Window *window, PM_Vec2 position, float radius,
                   PR_Color color);

/**
 * @brief Locks the window size to the size set while initialization.
 *
 * @param *window The window to lock the size.
 */
void PR_LockWindowSize(PR_Window *window);

#endif
