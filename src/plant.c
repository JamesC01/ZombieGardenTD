#include "plant.h"


int plantCooldownLUT[PT_COUNT] = {
    0, // PT_NONE,
    60*2, // PT_PSHOOTER,
    60*20, // PT_SUNFLOWER,
    0, //PT_WALLNUT,
    60*3 //PT_CHERRYBOMB,
    //PT_COUNT
};

int plantHealthLUT[PT_COUNT] = {
    0, // PT_NONE,
    100, // PT_PSHOOTER,
    100, // PT_SUNFLOWER,
    800, //PT_WALLNUT,
    100 //PT_CHERRYBOMB,
    //PT_COUNT
};
