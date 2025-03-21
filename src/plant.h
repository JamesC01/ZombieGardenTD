#ifndef PLANT_H
#define PLANT_H
#include <raylib.h>

typedef enum {
    PT_NONE,
    PT_SEEDSHOOTER,
    PT_SUNFLOWER,
    PT_COCONUT,
    PT_POTATOBOMB,
    PT_COUNT
} PlantType;

typedef struct {
    PlantType type;
    int cooldown;
    int flashTimer;
    float health;
} Plant;

// Cooldowns apply to certain actions, for peashooters, it's the shoot frequency,
// for sunflowers, is the spawnrate of the suns.
extern int plantCooldownMap[PT_COUNT];
extern int plantHealthMap[PT_COUNT];

void UpdateDrawPlants(void);

#endif
