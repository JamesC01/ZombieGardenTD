#ifndef ZOMBIE_H
#define ZOMBIE_H
#include <raylib.h>

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
extern int zombieSpawnTimer;
extern int zombieGrowlTimer;

void InitZombies(void);
void UpdateDrawZombies(void);

#endif
