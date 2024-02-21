#ifndef SUN_H
#define SUN_H
#include <raylib.h>

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

extern int sunsCollectedCount; // in PvZ, you start out with enough sun to buy a sunflower

void InitSuns();
void SpawnSun(Vector2 pos);
void UpdateDrawSuns(void);

#endif
