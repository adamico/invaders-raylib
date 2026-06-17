#include "raylib.h"
#include "resource_dir.h"
#include "scene_gameover.h"
#include "scene_gameplay.h"
#include "scene_title.h"
#include <stdio.h>

const Vector2 windowSize = {1280, 720};
const char *gameName = "Invaders RL";

void LoadGameResources(GameResources *resources) {
  resources->playerTexture = LoadTexture("player.png");
  resources->enemyTexture = LoadTexture("enemy.png");
  resources->laserTexture = LoadTexture("laser.png");
  resources->laserSound = LoadSound("laser.wav");
  resources->explosionSound = LoadSound("explosion.wav");
}

void UnloadGameResources(GameResources *resources) {
  UnloadTexture(resources->playerTexture);
  UnloadTexture(resources->enemyTexture);
  UnloadTexture(resources->laserTexture);
  UnloadSound(resources->laserSound);
  UnloadSound(resources->explosionSound);
}

// Main function
int main() {
#ifdef DEBUG
  printf(">>> ENGINE BOOTING IN DEBUG MODE\n");
#else
  printf(">>> ENGINE BOOTING IN RELEASE MODE\n");
#endif

  SearchAndSetResourceDir("resources");
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

  InitWindow(windowSize.x, windowSize.y, gameName);
  InitAudioDevice();

  GameState state = {0};

  LoadGameResources(&state.resources);
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

  UnloadGameResources(&state.resources);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}
