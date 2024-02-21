#ifndef GAME_H
#define GAME_H
#include <raylib.h>
#include <raymath.h>
#include "plant.h"
#include <stdbool.h>

extern const int screenWidth;
extern const int screenHeight;

void GameOver(void);

#define EXPAND_V2(v2) v2.x, v2.y

// Plant Grid globals
#define GRID_WIDTH 9
#define GRID_HEIGHT 5
extern Plant gardenGrid[GRID_WIDTH][GRID_HEIGHT];
extern Vector2 gridDrawOffset;
extern Vector2 gridCellSize;


typedef struct {
    Vector2 pos;
    bool active;
} Projectile;

// Projectile globals
#define MAX_PROJ 16
extern Projectile projectiles[MAX_PROJ];
extern int nextProjectile;

void NextObject(int *next, int max);
bool TickCooldown(int *timer, int cooldownTime);
int GetUniqueRandomValue(int exludedValue, int min, int max);
float GetRandomFloatValue(float min, float max);
void DrawTextureCentered(Texture2D sprite, Vector2 pos, Vector2 origin, Color tint);

#endif
