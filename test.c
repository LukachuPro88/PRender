#include <pmath.h>
#include <prender.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
  PM_Vec2 position;
  PM_Vec2 velocity;
  float radius;
  PR_Color color;
} Ball;

typedef struct {
  PM_Vec2 position;
  PM_Vec2 velocity;
  PM_Vec2 size;
  PR_Color color;
} Box;

int main(void) {
  PM_Vec2 win_pos = {100.0f, 100.0f};
  PR_Window *window = PR_InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                    "PRender Engine Test", win_pos);
  if (!window) {
    fprintf(stderr, "Failed to initialize application window.\n");
    return -1;
  }

  if (!PR_InitRenderer()) {
    fprintf(stderr, "Failed to initialize renderer or load shaders.\n");
    PR_KillWindow(window);
    return -1;
  }

  Ball ball = {
      .position = {200.0f, 300.0f},
      .velocity = {250.0f, -300.0f},
      .radius = 25.0f,
      .color = {1.0f, 0.0f, 0.0f, 1.0f} // Red
  };

  Box box = {
      .position = {500.0f, 100.0f},
      .velocity = {0.0f, 350.0f},
      .size = {60.0f, 60.0f},
      .color = {1.0f, 1.0f, 1.0f, 1.0f} // White
  };

  float gravity = 500.0f;
  float dt = 0.016f;

  while (!PR_WindowShouldClose(window)) {
    ball.velocity.y += gravity * dt;
    ball.position.x += ball.velocity.x * dt;
    ball.position.y += ball.velocity.y * dt;

    if (ball.position.x - ball.radius < 0.0f) {
      ball.position.x = ball.radius;
      ball.velocity.x *= -0.9f;
    } else if (ball.position.x + ball.radius > WINDOW_WIDTH) {
      ball.position.x = WINDOW_WIDTH - ball.radius;
      ball.velocity.x *= -0.9f;
    }
    if (ball.position.y - ball.radius < 0.0f) {
      ball.position.y = ball.radius;
      ball.velocity.y *= -0.9f;
    } else if (ball.position.y + ball.radius > WINDOW_HEIGHT) {
      ball.position.y = WINDOW_HEIGHT - ball.radius;
      ball.velocity.y *= -0.9f;
    }

    box.velocity.y += gravity * dt;
    box.position.y += box.velocity.y * dt;

    if (box.position.y < 0.0f) {
      box.position.y = 0.0f;
      box.velocity.y *= -0.9f;
    } else if (box.position.y + box.size.y > WINDOW_HEIGHT) {
      box.position.y = WINDOW_HEIGHT - box.size.y;
      box.velocity.y *= -0.9f;
    }

    PR_StartFrame((PR_Color){0.1f, 0.1f, 0.15f, 1.0f});

    PR_DrawQuad(window, box.position, box.size, box.color);

    PR_DrawCircle(window, ball.position, ball.radius, ball.color);

    PR_EndFrame(window);
  }

  PR_CleanRenderer();
  PR_KillWindow(window);

  return 0;
}
