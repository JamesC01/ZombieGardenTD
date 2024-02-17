#ifndef PLANT_H
#define PLANT_H
#include <raylib.h>

typedef enum {
    PT_NONE,
    PT_PSHOOTER,
    PT_SUNFLOWER,
    PT_WALLNUT,
    PT_CHERRYBOMB,
    PT_COUNT
} PlantType;

typedef struct {
    PlantType type;
    int cooldown;
    float health;
} Plant;

// Cooldowns apply to certain actions, for peashooters, it's the shoot frequency,
// for sunflowers, is the spawnrate of the suns.
extern int plantCooldownLUT[PT_COUNT];
extern int plantHealthLUT[PT_COUNT];

void UpdateDrawPlants();
void UpdateDrawPShooter(Plant* p, Vector2 gridPos, Vector2 screenPos);
void UpdateDrawSunflower(Plant* p, Vector2 screenPos);
void UpdateDrawCherryBomb(Plant* p, Vector2 gridPos, Vector2 screenPos);

#endif
