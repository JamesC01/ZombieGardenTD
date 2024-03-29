#ifndef ZOMBIE_H
#define ZOMBIE_H
#include "game.h"
#include <raylib.h>

typedef struct {
    Vector2 gridPos;
    float health;
    float scale;
    float rotation;
    bool headless;
    int headlessTimer;
    int flashTimer;
    bool active;
} Zombie;

typedef struct {
    Vector2 pos;
    Vector2 velocity;
    int floorY;
    int rowIndex;
    float rotation;
    float angularVel;
    bool active;
    bool onGround;
} ZombieHead;

extern FixedObjectArray zombies;
extern FixedObjectArray zombieHeads;

extern int zombiesKilledCount;
extern bool debugZombieSpawning;

void InitZombies(void);
void UpdateDrawZombies(void);
void UpdateDrawZombieHeads(void);

#endif
