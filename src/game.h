#ifndef GAME_H
#define GAME_H
#include <raylib.h>
#include <raymath.h>
#include "plant.h"
#include "ui.h"
#include <stdbool.h>

extern const int virtualScreenWidth;
extern const int virtualScreenHeight;

void GameOver(void);

#define EXPAND_V2(v2) v2.x, v2.y

#define VERSION_STRING "v1.0"

// Plant Grid globals
#define GRID_WIDTH 9
#define GRID_HEIGHT 5
extern Plant gardenGrid[GRID_WIDTH][GRID_HEIGHT];
extern Vector2 gridDrawOffset;
extern Vector2 gridCellSize;


typedef struct {
    int fixedSize;
    void* array;
    int next;
} FixedObjectArray;


typedef struct {
    Vector2 pos;
    bool active;
} Projectile;

extern FixedObjectArray projectiles;

typedef struct {
    Texture sprite;
    Vector2 pos;
    Vector2 origin;
    Color tint;
    float scale;
    float rotation;
    bool shouldDraw;
    int depth;
} DrawData;

extern FixedObjectArray drawDatas;

typedef enum {
    LAYER_PLANTS = 10,
    LAYER_ZOMBIES = 20,
} DrawLayer;


#define GetNextObject(fixedObjArray, type) &((type*)fixedObjArray.array)[fixedObjArray.next]
void IncrementArrayIndex(FixedObjectArray *array);

bool TickCooldown(int *timer, int cooldownTime);
int GetUniqueRandomValue(int exludedValue, int min, int max);
float GetRandomFloatValue(float min, float max);
void DrawTextureFull(Texture2D sprite, Vector2 pos, Vector2 origin, Color tint, float scale, float rotation);
void PushDrawData(Texture2D sprite, int depth, Vector2 pos, Vector2 origin, Color tint, float scale, float rotation);
Vector2 GetTextureCenterPoint(Texture2D sprite);
Vector2 GetMousePosVirtual(void);
int GetButtonHeight(ButtonOptions bOpt, TextOptions tOpt);

#endif
