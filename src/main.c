/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit
https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#define MAX_PROJECTILES 100

const Vector2 windowSize = {1280, 720};

// Player definition
typedef struct Player {
  Vector2 pos;
  float radius;
  Color color;
  float speed;
  Vector2 dir;
} Player;

void UpdatePlayer(Player *player, float dt) {
  float radius = player->radius;
  float speed = player->speed;
  player->dir = Vector2Zero();
  Vector2 *playerPos = &player->pos;
  Vector2 *playerDir = &player->dir;

  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    playerDir->x -= 1.0f;
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    playerDir->x += 1.0f;
  if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    playerDir->y -= 1.0f;
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    playerDir->y += 1.0f;

  *playerDir = Vector2Normalize(*playerDir);
  *playerPos = Vector2Add(*playerPos, Vector2Scale(*playerDir, speed * dt));
  *playerPos =
      Vector2Clamp(*playerPos, (Vector2){radius, radius},
                   (Vector2){windowSize.x - radius, windowSize.y - radius});
};

void DrawPlayer(Player *player) {
  DrawCircleV(player->pos, player->radius, player->color);
};

// Bullet
typedef struct Projectile {
  Vector2 pos;
  Vector2 dir;
  float speed;
  float radius;
  bool active;
} Projectile;

void PlayerShoot(Player *player, Projectile *bullets) {
  if (IsKeyPressed(KEY_SPACE)) {
    for (int bulletIndex = 0; bulletIndex < MAX_PROJECTILES; bulletIndex++) {
      Projectile *bullet = &bullets[bulletIndex];

      if (!bullet->active) {
        bullet->active = true;
        bullet->pos = player->pos;
        bullet->dir = (Vector2){0.0f, -1.0f};
        bullet->speed = 500.0f;
        bullet->radius = 5.0f;
        break;
      }
    }
  }
}

void UpdateProjectiles(Projectile *bullets, float dt) {
  for (int bulletIndex = 0; bulletIndex < MAX_PROJECTILES; bulletIndex++) {
    Projectile *bullet = &bullets[bulletIndex];
    if (bullet->active) {
      bullet->pos = Vector2Add(bullet->pos,
                               Vector2Scale(bullet->dir, bullet->speed * dt));
      if (bullet->pos.y < 0)
        bullet->active = false;
    }
  }
}

void DrawProjectiles(Projectile *bullets) {
  for (int bulletIndex = 0; bulletIndex < MAX_PROJECTILES; bulletIndex++) {
    Projectile *bullet = &bullets[bulletIndex];
    if (bullet->active) {
      DrawCircleV(bullet->pos, bullet->radius, RED);
    }
  }
}

// Helper to draw text horizontally centered on screen
void CenterText(const char *text, int yPos, int fontSize, Color textColor) {
  int textSize = MeasureText(text, fontSize);
  DrawText(text, (int)windowSize.x / 2 - textSize / 2, yPos, fontSize,
           textColor);
}

// Main function
int main() {
  // Tell the window to use vsync and work on high DPI displays
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

  // Create the window and OpenGL context
  InitWindow(windowSize.x, windowSize.y, "Hello Raylib");

  // Utility function from resource_dir.h to find the resources folder and set
  // it as the current working directory so we can load from it
  SearchAndSetResourceDir("resources");

  Projectile bullets[MAX_PROJECTILES] = {0};

  Player player = {.pos = {windowSize.x / 2, windowSize.y / 2},
                   .radius = 25.0f,
                   .color = GREEN,
                   .speed = 300.0f,
                   .dir = {0.0f, 0.0f}};

  // game loop
  // run the loop until the user presses ESCAPE or presses the Close button on
  // the window
  while (!WindowShouldClose()) {
    // update
    float dt = GetFrameTime();
    UpdatePlayer(&player, dt);

    PlayerShoot(&player, bullets);
    UpdateProjectiles(bullets, dt);

    // draw
    BeginDrawing();

    // Setup the back buffer for drawing (clear color and depth buffers)
    ClearBackground(BLACK);

    int font_size = 20;
    CenterText("Hello Raylib, wassup?", 100, font_size, WHITE);
    const char *screenSizeText =
        TextFormat("Screen Size %ix%i", GetScreenWidth(), GetScreenHeight());
    CenterText(screenSizeText, 220, font_size, WHITE);

    DrawPlayer(&player);
    DrawProjectiles(bullets);

    // draw debug
    DrawText(TextFormat("Player Position: %i/%i", (int)player.pos.x,
                        (int)player.pos.y),
             20, 20, 20, RED);
    DrawText(
        TextFormat("Player Direction: %.2f/%.2f", player.dir.x, player.dir.y),
        20, 40, 20, RED);

    // end the frame and get ready for the next one  (display frame, poll
    // input, etc...)
    EndDrawing();
  }

  // cleanup

  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
