#ifndef ZOMBIE_H
#define ZOMBIE_H
#include "game.h"
#include <raylib.h>

typedef struct {
    Vector2 gridPos;
    float health;
    bool active;
} Zombie;

extern FixedObjectArray zombies;

extern int zombiesKilledCount;

void InitZombies(void);
void UpdateDrawZombies(void);

#endif
