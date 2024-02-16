#ifndef PLANT_H
#define PLANT_H

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

#endif
