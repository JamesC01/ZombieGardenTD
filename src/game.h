#ifndef GAME_H
#define GAME_H
#include <raylib.h>
#include <raymath.h>
#include "plant.h"
#include <stdbool.h>

extern const int virtualScreenWidth;
extern const int virtualScreenHeight;

void GameOver(void);

#define EXPAND_V2(v2) v2.x, v2.y

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

// Get a pointer to the next free FixedObjectArray object
#define GET_NEXT_OBJECT(fixedObjArray, type) &((type*)(fixedObjArray).array)[(fixedObjArray).next]

typedef struct {
    Vector2 pos;
    bool active;
} Projectile;

extern FixedObjectArray projectiles;


typedef struct {
    Font font;
    int size;
    int shadowOffset;
    Color colour;
} TextOptions;

void NextObject(FixedObjectArray *array);
bool TickCooldown(int *timer, int cooldownTime);
int GetUniqueRandomValue(int exludedValue, int min, int max);
float GetRandomFloatValue(float min, float max);
void DrawTextureCentered(Texture2D sprite, Vector2 pos, Vector2 origin, Color tint);
Vector2 GetMousePosVirtual(void);
bool TextButton(TextOptions textOptions, char *text, int x, int y, Color buttonColor, int buttonShadowOffset);

void DrawTextWithShadow(Font font, const char *text, int x, int y, float fontSize, float shadowOffset, Color tint);

#endif
