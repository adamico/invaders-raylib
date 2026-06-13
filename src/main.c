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

const Vector2 windowSize = {1280, 720};

// Player definition
typedef struct Player {
  Vector2 pos;
  float radius;
  Color color;
  float speed;
  Vector2 dir;
} Player;

// Helper to draw text horizontally centered on screen
void CenterText(const char *text, int yPos, int fontSize, Color textColor) {
  int textSize = MeasureText(text, fontSize);
  DrawText(text, (int)windowSize.x / 2 - textSize / 2, yPos, fontSize,
           textColor);
}

void UpdatePlayer(Player *player, float dt) {
  player->dir = (Vector2){0.0f, 0.0f};
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    player->dir.x -= 1.0f;
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    player->dir.x += 1.0f;
  if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    player->dir.y -= 1.0f;
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    player->dir.y += 1.0f;

  player->dir = Vector2Normalize(player->dir);

  player->pos.x += player->dir.x * player->speed * dt;
  player->pos.y += player->dir.y * player->speed * dt;

  player->pos.x =
      Clamp(player->pos.x, player->radius, windowSize.x - player->radius);
  player->pos.y =
      Clamp(player->pos.y, player->radius, windowSize.y - player->radius);
};

void DrawPlayer(Player *player) {
    DrawCircleV(player->pos, player->radius, player->color);
};

// Main function
int main() {
  // Tell the window to use vsync and work on high DPI displays
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

  // Create the window and OpenGL context
  InitWindow(windowSize.x, windowSize.y, "Hello Raylib");

  // Utility function from resource_dir.h to find the resources folder and set
  // it as the current working directory so we can load from it
  SearchAndSetResourceDir("resources");

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

    // draw debug
    DrawText(TextFormat("Player Position: %i/%i", (int)player.pos.x,
                        (int)player.pos.y),
             20, 20, 20, RED);
    DrawText(
        TextFormat("Player Direction: %.2f/%.2f", player.dir.x, player.dir.y),
        20, 40, 20, RED);

    // end the frame and get ready for the next one  (display frame, poll input,
    // etc...)
    EndDrawing();
  }

  // cleanup

  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
