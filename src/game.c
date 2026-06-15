#include "game.h"

#include "raylib.h"
#include "raymath.h"
#include <stdarg.h>
#include <stdio.h>

#define MAX_PROJECTILES 100
#define MAX_ENEMIES 55

#define MAX_ENEMIES_PER_ROW 11
#define COL_PADDING 80
#define ROW_PADDING 60
#define ENEMY_SPEED 50.0f

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


void InitPlayer(GameState *state) {
  state->player =
      (Player){.pos = {windowSize.x / 2, windowSize.y - (PLAYER_RADIUS * 2)},
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
    state->enemyDirection = (Vector2){1.0f, 0.0f};
    state->enemySpeed = ENEMY_SPEED;
    state->activeEnemies++;
  }
}

void UpdateEnemies(GameState *state, float dt) {
  Vector2 direction = state->enemyDirection;
  int speed = state->enemySpeed;
  bool needToMoveDown = false;
  // enemies first move right
  // until the right most enemy column reaches the edge of the screen,
  // these two checks need to be inversed in two different loops

  FOR_EACH_ENEMY(enemy, state->enemies) {
    if (!enemy->active)
      continue;

    bool willHitRightEdge =
        (enemy->pos.x + enemy->radius) >= windowSize.x && direction.x > 0;
    bool willHitLeftEdge =
        (enemy->pos.x - enemy->radius) <= 0 && direction.x < 0;
    if (willHitRightEdge || willHitLeftEdge) {
      state->enemyDirection.x *= -1.0f;
      needToMoveDown = true;
      break;
    }
  }

  if (needToMoveDown) {
    FOR_EACH_ENEMY(enemy, state->enemies) {
      enemy->pos.y += ROW_PADDING / 2.0;
      if (enemy->pos.y > windowSize.y) {
        state->currentScene = GAMEOVER;
        break;
      }
    }
    needToMoveDown = false;
  }

  FOR_EACH_ENEMY(enemy, state->enemies) {
    if (!enemy->active)
      continue;

    Vector2 *enemyPos = &enemy->pos;
    *enemyPos = Vector2Add(*enemyPos, Vector2Scale(direction, speed * dt));
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
        state->activeEnemies--;
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
      state->currentScene = GAMEOVER;
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
  state->victory = false;
  InitPlayer(state);
  InitEnemies(state);
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

void CheckIfPlayerDied(GameState *state) {
  if (state->player.health <= 0)
    state->currentScene = GAMEOVER;
}

void CheckIfPlayerWon(GameState *state) {
  if (state->activeEnemies <= 0) {
    state->victory = true;
    state->currentScene = GAMEOVER;
  }
}

void UpdateGame(GameState *state, float dt) {
  CheckIfPlayerDied(state);
  CheckIfPlayerWon(state);
  UpdatePlayer(state, dt);
  UpdateEnemies(state, dt);
  PlayerShoot(state);
  UpdateProjectiles(state, dt);
  CheckBulletEnemyCollisions(state);
  CheckPlayerEnemyCollisions(state);
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

  // draw active enemies
  DrawText(TextFormat("Active Enemies: %i", state->activeEnemies), 20, 80,
           font_size, RED);

  // draw debug
  DrawText(TextFormat("Player Position: %i/%i", (int)state->player.pos.x,
                      (int)state->player.pos.y),
           20, 20, font_size, RED);
  DrawText(TextFormat("Player Direction: %.2f/%.2f", state->player.dir.x,
                      state->player.dir.y),
           20, 40, font_size, RED);
  EndDrawing();
}

void UpdateGameover(GameState *state) {
  if (IsKeyPressed(KEY_ENTER)) {
    state->currentScene = TITLE;
    InitGame(state);
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
