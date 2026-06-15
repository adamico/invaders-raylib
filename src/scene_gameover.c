#include "scene_gameover.h"

void UpdateGameover(GameState *state) {
  if (IsKeyPressed(KEY_ENTER)) {
    state->currentScene = TITLE;
    InitGameplay(state);
  }
}
void DrawGameover(GameState *state) {
  BeginDrawing();
  ClearBackground(BLACK);
  char *gameOverText = "Game Over";
  Color textColor = RED;

  if (state->victory) {
    gameOverText = "You Win!";
    textColor = GREEN;
  }
  CenterText(gameOverText, windowSize.y / 2, 50, textColor);
  CenterText("Press Enter to go to Title", windowSize.y / 2 + 100, 20,
             textColor);
  EndDrawing();
}
