#include "globals.h"
#include "assets.h"
#include <stdlib.h>

Plant gardenGrid[GRID_WIDTH][GRID_HEIGHT] = {0};
Vector2 gridDrawOffset = {40, 80};
Vector2 gridCellSize = {65, 78};


Sun suns[MAX_SUNS] = {0};
int nextSun = 0;

int sunCooldown = 60;
int sunsCollectedCount = SUN_VALUE*2; // in PvZ, you start out with enough sun to buy a sunflower



Projectile projectiles[MAX_PROJ] = {0};
int nextProjectile = 0;

int GetUniqueRandomValue(int exludedValue, int min, int max)
{
    int random;
    do {
        random = GetRandomValue(min, max);
    } while (random == exludedValue);

    return random;
}

void SpawnSun(Vector2 pos)
{
    if (nextSun == MAX_SUNS) {
        nextSun = 0;
    }

    suns[nextSun].active = true;
    suns[nextSun].pos = pos;
    PlaySound(sunAppearSound);

    nextSun++;
}


void DrawTextureCentered(Texture2D sprite, Vector2 pos, Vector2 origin, Color tint)
{
    Rectangle src = {0, 0, sprite.width, sprite.height};
    Rectangle dst = {EXPAND_V2(pos), sprite.width, sprite.height};
    DrawTexturePro(sprite, src, dst, origin, 0, tint);
}
