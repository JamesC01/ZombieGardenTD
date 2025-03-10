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

#define VERSION_STRING "v1.0.1"

extern const int SPRITE_FLASH_TIME;

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
    LAYER_UI = 0,
    LAYER_PLANTS = 10,
    LAYER_ZOMBIES = 20, // zombie render layer is calculated as LAYER_ZOMBIES + row * 10
    LAYER_UNUSED = 80
} DrawLayer;

typedef struct {
    bool playingMusic;
    bool raining;
    bool fullscreen;
    int width, height;
    bool maximized;
} GameConfig;

extern GameConfig gameConfig;

typedef enum {
    GAME_SCREEN_START,
    GAME_SCREEN_PLAYING,
    GAME_SCREEN_PAUSE_MENU,
    GAME_SCREEN_GAMEOVER,
    GAME_SCREEN_CONFIG_MENU,
    GAME_SCREEN_DEBUG_MENU,
    GAME_SCREEN_EXIT
} GameScreen;


#define GetNextObject(fixedObjArray, type) &((type*)fixedObjArray.array)[fixedObjArray.next]
void IncrementArrayIndex(FixedObjectArray *array);

bool TickCooldown(int *timer, int cooldownTime);
int GetUniqueRandomValue(int exludedValue, int min, int max);
float GetRandomFloatValue(float min, float max);
Color GetFlashTint(int flashTimer);
void DrawTextureFull(Texture2D sprite, Vector2 pos, Vector2 origin, Color tint, float scale, float rotation);
void PushDrawData(Texture2D sprite, int depth, Vector2 pos, Vector2 origin, Color tint, float scale, float rotation);
Vector2 GetTextureCenterPoint(Texture2D sprite);
Vector2 GetMousePosVirtual(void);
int GetButtonHeight(ButtonOptions bOpt, TextOptions tOpt);
void DrawTitleText(char *text);
void ChangeGameScreen(GameScreen newScreen);
void InitializeGame(void);

#endif
