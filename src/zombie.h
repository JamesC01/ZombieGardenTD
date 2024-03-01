#ifndef ZOMBIE_H
#define ZOMBIE_H
#include "game.h"
#include <raylib.h>

typedef struct {
    Vector2 gridPos;
    float health;
    float scale;
    bool headless;
    int headlessTimer;
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

void InitZombies(void);
void UpdateDrawZombies(void);
void UpdateDrawZombieHeads(void);

#endif
