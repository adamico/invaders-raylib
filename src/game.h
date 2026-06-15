#ifndef GAME_H
#define GAME_H

#include <raylib.h>

#define MAX_PROJECTILES 100
#define MAX_ENEMIES 55

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

void InitGame(GameState *state);

void UpdateTitle(GameState *state);
void UpdateGame(GameState *state, float dt);
void UpdateGameover(GameState *state);

void DrawTitle(GameState *state);
void DrawGame(GameState *state);
void DrawGameover(GameState *state);

#endif
