#include "raylib.h"
#include "raymath.h"
#include <stdarg.h>
#include <stdio.h>

#include "game.h"
#include "scene_gameplay.h"

#define CANVAS_SIZE 50

#define CANVAS_OFFSET (Vector2){-CANVAS_SIZE / 2.0, -CANVAS_SIZE / 2.0}

#define PLAYER_RADIUS 12.5f
#define PLAYER_SPEED 300.0f
#define PLAYER_HEALTH 5
#define INITIAL_PLAYER_POS                                                     \
  (Vector2) { windowSize.x / 2, windowSize.y - (PLAYER_RADIUS * 4) }

#define MAX_ENEMIES_PER_ROW 11
#define COL_PADDING 80
#define ROW_PADDING 60
#define ENEMY_SPEED 50.0f
#define ENEMY_RADIUS 15.0f
#define ENEMY_SCORE_VALUE 100
#define START_GRID_POS                                                         \
  (Vector2){((windowSize.x - (MAX_ENEMIES_PER_ROW * COL_PADDING)) / 2) +       \
                (COL_PADDING / 2.0),                                           \
            100}

#define FOR_EACH_PROJECTILE(projectilePtr, projectileArray)                    \
  for (Projectile *projectilePtr = projectileArray;                            \
       projectilePtr < projectileArray + MAX_PROJECTILES; projectilePtr++)

#define FOR_EACH_ENEMY(enemyPtr, enemyArray)                                   \
  for (Enemy *enemyPtr = enemyArray; enemyPtr < enemyArray + MAX_ENEMIES;      \
       enemyPtr++)

void InitPlayer(GameState *state) {
  state->player = (Player){.pos = INITIAL_PLAYER_POS,
                           .radius = PLAYER_RADIUS,
                           .speed = PLAYER_SPEED,
                           .dir = {0.0f, 0.0f},
                           .health = PLAYER_HEALTH};
}

void InitEnemies(GameState *state) {
  for (int enemyIndex = 0; enemyIndex < MAX_ENEMIES; enemyIndex++) {
    int column = enemyIndex % MAX_ENEMIES_PER_ROW;
    int row = enemyIndex / MAX_ENEMIES_PER_ROW;
    int offsetX = START_GRID_POS.x;
    int offsetY = START_GRID_POS.y;
    Enemy enemy = {.pos = (Vector2){offsetX + (column * COL_PADDING),
                                    offsetY + (row * ROW_PADDING)},
                   .radius = ENEMY_RADIUS,
                   .active = true,
                   .scoreValue = ENEMY_SCORE_VALUE};
    state->enemies[enemyIndex] = enemy;
    state->enemyDirection = (Vector2){1.0f, 0.0f};
    state->enemySpeed = ENEMY_SPEED;
    state->activeEnemies++;
  }
}

void InitGameplay(GameState *state) {
  GameResources resBackup = state->resources;
  *state = (GameState){0};
  state->resources = resBackup;

  state->victory = false;
  InitPlayer(state);
  InitEnemies(state);
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

void DrawOffset(Texture2D texture, Vector2 pos, Color tint) {
  Vector2 drawPos = Vector2Add(pos, CANVAS_OFFSET);
  DrawTextureV(texture, drawPos, tint);
}

void DrawPlayer(GameState *state) {
  DrawOffset(state->resources.playerTexture, state->player.pos, WHITE);
  DrawCircleLinesV(state->player.pos, state->player.radius, RED);
};

void PlayerShoot(GameState *state) {
  if (!IsKeyPressed(KEY_SPACE))
    return;

  PlaySound(state->resources.laserSound);
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
      DrawOffset(state->resources.laserTexture, bullet->pos, WHITE);
      DrawCircleLinesV(bullet->pos, bullet->radius, RED);
    }
  }
}

void UpdateEnemies(GameState *state, float dt) {
  Vector2 direction = state->enemyDirection;
  int speed = state->enemySpeed;
  bool needToMoveDown = false;
  int rightEdge = windowSize.x - ENEMY_RADIUS;
  int leftEdge = ENEMY_RADIUS;
  FOR_EACH_ENEMY(enemy, state->enemies) {
    if (!enemy->active)
      continue;

    bool willHitRightEdge =
        (enemy->pos.x + enemy->radius) >= rightEdge && direction.x > 0;
    bool willHitLeftEdge =
        (enemy->pos.x - enemy->radius) <= leftEdge && direction.x < 0;
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
    if (enemy->active) {
      DrawOffset(state->resources.enemyTexture, enemy->pos, WHITE);
      DrawCircleLinesV(enemy->pos, enemy->radius, RED);
    }
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
        PlaySound(state->resources.explosionSound);
        bullet->active = false;
        enemy->active = false;
        state->activeEnemies--;
        state->score += enemy->scoreValue;
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

void UpdateGameplay(GameState *state, float dt) {
  CheckIfPlayerDied(state);
  CheckIfPlayerWon(state);
  UpdatePlayer(state, dt);
  UpdateEnemies(state, dt);
  PlayerShoot(state);
  UpdateProjectiles(state, dt);
  CheckBulletEnemyCollisions(state);
  CheckPlayerEnemyCollisions(state);
}

void DrawGameplay(GameState *state) {
  BeginDrawing();

  // Setup the back buffer for drawing (clear color and depth buffers)
  ClearBackground(BLACK);

  int font_size = 20;

  DrawPlayer(state);
  DrawEnemies(state);
  DrawProjectiles(state);

  DrawText(TextFormat("Health: %i", state->player.health), 20,
           windowSize.y - 40, font_size, WHITE);

  DrawText(TextFormat("Score: %i", state->score), 20, 20, font_size, WHITE);
  EndDrawing();
}
