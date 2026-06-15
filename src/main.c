/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit
https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "raymath.h"
#include <stdarg.h>
#include <stdio.h>

#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#define MAX_PROJECTILES 100
#define MAX_ENEMIES 55
#define MAX_ENEMIES_PER_ROW 11
#define COL_PADDING 80
#define ROW_PADDING 60

#define PLAYER_HEALTH 5
#define PLAYER_RADIUS 25.0f

#define FOR_EACH_PROJECTILE(projectilePtr, projectileArray)                    \
  for (Projectile *projectilePtr = projectileArray;                            \
       projectilePtr < projectileArray + MAX_PROJECTILES; projectilePtr++)

#define FOR_EACH_ENEMY(enemyPtr, enemyArray)                                   \
  for (Enemy *enemyPtr = enemyArray; enemyPtr < enemyArray + MAX_ENEMIES;      \
       enemyPtr++)

const Vector2 windowSize = {1280, 720};
const Vector2 startGridPos = {
    ((windowSize.x - (MAX_ENEMIES_PER_ROW * COL_PADDING)) / 2) +
        (COL_PADDING / 2.0),
    100};

typedef enum GameScene { TITLE, GAMEPLAY, GAMEOVER } GameScene;

// Player definition
typedef struct Player {
  Vector2 pos;
  float radius;
  Color color;
  float speed;
  Vector2 dir;
  int health;
} Player;

// Bullet
typedef struct Projectile {
  Vector2 pos;
  Vector2 dir;
  float speed;
  float radius;
  bool active;
} Projectile;

// Enemies
typedef struct Enemy {
  Vector2 pos;
  float radius;
  bool active;
  Color color;
} Enemy;

typedef struct GameState {
  Player player;
  Projectile bullets[MAX_PROJECTILES];
  Enemy enemies[MAX_ENEMIES];
} GameState;

void InitPlayer(GameState *state) {
  state->player = (Player){.pos = {windowSize.x / 2, windowSize.y - (PLAYER_RADIUS * 2)},
                           .radius = PLAYER_RADIUS,
                           .color = GREEN,
                           .speed = 300.0f,
                           .dir = {0.0f, 0.0f},
                           .health = 5};
}

void UpdatePlayer(GameState *state, float dt) {
  float radius = state->player.radius;
  float speed = state->player.speed;
  state->player.dir = Vector2Zero();
  Vector2 *playerPos = &state->player.pos;
  Vector2 *playerDir = &state->player.dir;

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

void DrawPlayer(GameState *state) {
  DrawCircleV(state->player.pos, state->player.radius, state->player.color);
};

void PlayerShoot(GameState *state) {
  if (!IsKeyPressed(KEY_SPACE))
    return;

  FOR_EACH_PROJECTILE(bullet, state->bullets) {
    if (bullet->active)
      continue;

    bullet->active = true;
    bullet->pos = state->player.pos;
    bullet->dir = (Vector2){0.0f, -1.0f};
    bullet->speed = 500.0f;
    bullet->radius = 5.0f;
    break;
  }
}

void UpdateProjectiles(GameState *state, float dt) {
  FOR_EACH_PROJECTILE(bullet, state->bullets) {
    if (!bullet->active)
      continue;
    bullet->pos =
        Vector2Add(bullet->pos, Vector2Scale(bullet->dir, bullet->speed * dt));
    bullet->active = bullet->pos.y > 0;
  }
}

void DrawProjectiles(GameState *state) {
  FOR_EACH_PROJECTILE(bullet, state->bullets) {
    if (bullet->active) {
      DrawCircleV(bullet->pos, bullet->radius, RED);
    }
  }
}

void InitEnemies(GameState *state) {
  for (int enemyIndex = 0; enemyIndex < MAX_ENEMIES; enemyIndex++) {
    int column = enemyIndex % MAX_ENEMIES_PER_ROW;
    int row = enemyIndex / MAX_ENEMIES_PER_ROW;
    int offsetX = startGridPos.x;
    int offsetY = startGridPos.y;
    Enemy enemy = {.pos = (Vector2){offsetX + (column * COL_PADDING),
                                    offsetY + (row * ROW_PADDING)},
                   .radius = 20,
                   .active = true,
                   .color = BLUE};
    state->enemies[enemyIndex] = enemy;
  }
}

void DrawEnemies(GameState *state) {
  FOR_EACH_ENEMY(enemy, state->enemies) {
    if (enemy->active)
      DrawCircleV(enemy->pos, enemy->radius, enemy->color);
  }
}

void CheckBulletEnemyCollisions(GameState *state) {
  FOR_EACH_PROJECTILE(bullet, state->bullets) {
    if (!bullet->active)
      continue;

    FOR_EACH_ENEMY(enemy, state->enemies) {
      if (!enemy->active)
        continue;

      if (CheckCollisionCircles(bullet->pos, bullet->radius, enemy->pos,
                                enemy->radius)) {
        bullet->active = false;
        enemy->active = false;
      }
    }
  }
}

void CheckPlayerEnemyCollisions(GameState *state) {
  FOR_EACH_ENEMY(enemy, state->enemies) {
    if (!enemy->active)
      continue;

    if (CheckCollisionCircles(state->player.pos, state->player.radius,
                              enemy->pos, enemy->radius)) {
      enemy->active = false;
      state->player.health -= 1;
    }
  }
}

// Helper to draw text horizontally centered on screen
void CenterText(const char *text, int yPos, int fontSize, Color textColor) {
  int textSize = MeasureText(text, fontSize);
  DrawText(text, (int)windowSize.x / 2 - textSize / 2, yPos, fontSize,
           textColor);
}

void InitGame(GameState *state) {
  *state = (GameState){0};
  InitPlayer(state);
  InitEnemies(state);
}

void UpdateTitle(GameState *state, GameScene *currentScene) {
  if (IsKeyPressed(KEY_SPACE))
    *currentScene = GAMEPLAY;
}

void DrawTitle(GameState *state) {
  BeginDrawing();
  ClearBackground(BLACK);
  CenterText("Invaders RL", windowSize.y / 2, 40, WHITE);
  CenterText("Press Space to Start", windowSize.y / 2 + 100, 20, WHITE);
  EndDrawing();
}

void CheckIfPlayerDied(GameState *state, GameScene *currentScene) {
  if (state->player.health <= 0)
    *currentScene = GAMEOVER;
}

void UpdateGame(GameState *state, GameScene *currentScene, float dt) {
  UpdatePlayer(state, dt);
  PlayerShoot(state);
  UpdateProjectiles(state, dt);
  CheckBulletEnemyCollisions(state);
  CheckPlayerEnemyCollisions(state);
  CheckIfPlayerDied(state, currentScene);
}

void DrawGame(GameState *state) {
  BeginDrawing();

  // Setup the back buffer for drawing (clear color and depth buffers)
  ClearBackground(BLACK);

  int font_size = 20;

  DrawPlayer(state);
  DrawEnemies(state);
  DrawProjectiles(state);

  // draw health
  DrawText(TextFormat("Health: %i", state->player.health), 20, 60, font_size,
           RED);

  // draw debug
  DrawText(TextFormat("Player Position: %i/%i", (int)state->player.pos.x,
                      (int)state->player.pos.y),
           20, 20, font_size, RED);
  DrawText(TextFormat("Player Direction: %.2f/%.2f", state->player.dir.x,
                      state->player.dir.y),
           20, 40, font_size, RED);

  // end the frame and get ready for the next one  (display frame, poll
  // input, etc...)
  EndDrawing();
}

void UpdateGameOver(GameState *state, GameScene *currentScene) {
  if (IsKeyPressed(KEY_ENTER)) {
    *currentScene = TITLE;
    InitGame(state);
  }
}

void DrawGameOver(GameState *state) {
  BeginDrawing();
  ClearBackground(BLACK);
  CenterText("Game Over", windowSize.y / 2, 50, RED);
  CenterText("Press Enter to go to Title", windowSize.y / 2 + 100, 20, RED);
  EndDrawing();
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

  GameState state = {0};
  InitGame(&state);

  GameScene currentScene = TITLE;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    switch (currentScene) {
    case TITLE:
      UpdateTitle(&state, &currentScene);
      DrawTitle(&state);
      break;
    case GAMEPLAY:
      UpdateGame(&state, &currentScene, dt);
      DrawGame(&state);
      break;
    case GAMEOVER:
      UpdateGameOver(&state, &currentScene);
      DrawGameOver(&state);
      break;
    }
  }

  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
