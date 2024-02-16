#ifndef GLOBALS_H
#define GLOBALS_H
#include <raylib.h>
#include "plant.h"

#define EXPAND_V2(v2) v2.x, v2.y

typedef struct {
    Vector2 pos;
    bool active;
} Sun;

// Sun globals
#define SUN_VALUE 25
#define SUN_SPAWN_TIME 60*15
#define MAX_SUNS 8
extern Sun suns[MAX_SUNS];
extern int nextSun;

extern int sunCooldown;
extern int sunsCollectedCount; // in PvZ, you start out with enough sun to buy a sunflower


typedef struct {
    Vector2 gridPos;
    float health;
    bool active;
} Zombie;

// Zombie globals
#define MAX_ZOMBIES 32
extern Zombie zombies[MAX_ZOMBIES];
extern int nextZombie;

extern int currentZombieSpawnRate;
extern int zombieSpawnCooldown;
extern int zombieGrowlCooldown;


// Plant Grid globals
#define GRID_WIDTH 9
#define GRID_HEIGHT 5
extern Plant gardenGrid[GRID_WIDTH][GRID_HEIGHT];
extern Vector2 gridDrawOffset;
extern Vector2 gridCellSize;



#endif
