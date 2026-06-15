#include "raylib.h"

#ifndef GAME_H
#define GAME_H

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

extern const Vector2 windowSize;

// Helper to draw text horizontally centered on screen
void CenterText(const char *text, int yPos, int fontSize, Color textColor);

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
  Vector2 enemyDirection;
  int enemySpeed;
  int activeEnemies;
  GameScene currentScene;
  bool victory;
} GameState;

void InitGameplay(GameState *state);

#endif
