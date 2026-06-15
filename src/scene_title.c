#include "raylib.h"

#include "game.h"
#include "scene_title.h"

void CenterText(const char *text, int yPos, int fontSize, Color textColor) {
  int textSize = MeasureText(text, fontSize);
  DrawText(text, (int)windowSize.x / 2 - textSize / 2, yPos, fontSize,
           textColor);
}

void UpdateTitle(GameState *state) {
  if (IsKeyPressed(KEY_SPACE))
    state->currentScene = GAMEPLAY;
}

void DrawTitle(GameState *state) {
  BeginDrawing();
  ClearBackground(BLACK);
  CenterText("Invaders RL", windowSize.y / 2, 40, WHITE);
  CenterText("Press Space to Start", windowSize.y / 2 + 100, 20, WHITE);
  EndDrawing();
}
