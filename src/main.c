/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit
https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "scene_title.h"
#include "scene_gameplay.h"
#include "scene_gameover.h"

const Vector2 windowSize = {1280, 720};

// Main function
int main() {
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

  InitWindow(1280, 720, "Invaders RL");

  GameState state = {0};
  InitGameplay(&state);

  state.currentScene = TITLE;

  while (!WindowShouldClose()) {
    switch (state.currentScene) {
    case TITLE:
      UpdateTitle(&state);
      DrawTitle(&state);
      break;
    case GAMEPLAY:
      UpdateGameplay(&state, GetFrameTime());
      DrawGameplay(&state);
      break;
    case GAMEOVER:
      UpdateGameover(&state);
      DrawGameover(&state);
      break;
    }
  }

  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
