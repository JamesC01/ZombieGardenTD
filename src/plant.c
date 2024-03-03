#include "plant.h"
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "assets.h"
#include "particles.h"
#include "zombie.h"
#include "sun.h"

int plantCooldownLUT[PT_COUNT] = {
    0, // PT_NONE,
    60*2, // PT_PSHOOTER,
    60*25, // PT_SUNFLOWER,
    0, //PT_WALLNUT,
    60*2 //PT_CHERRYBOMB,
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

void UpdateDrawSeedShooter(Plant* p, Vector2 gridPos, Vector2 screenPos);
void UpdateDrawSunflower(Plant* p, Vector2 screenPos);
void UpdateDrawPotatoBomb(Plant* p, Vector2 gridPos, Vector2 screenPos);

void UpdateDrawPlants()
{
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            if (gardenGrid[x][y].type != PT_NONE) {
                Vector2 screenPos = Vector2Add(gridDrawOffset, (Vector2){x*gridCellSize.x+gridCellSize.x/2, y*gridCellSize.y+gridCellSize.y*0.75f});
                Plant* p = &gardenGrid[x][y];

                if (p->health <= 0) {
                    p->type = PT_NONE;
                }


                PushDrawData(shadowSprite, LAYER_PLANTS-1, screenPos, GetTextureCenterPoint(shadowSprite), WHITE, 1, 0);

                switch (p->type) {
                    case PT_SEEDSHOOTER:
                        UpdateDrawSeedShooter(p, (Vector2){x, y}, screenPos);
                        break;
                    case PT_SUNFLOWER:
                        UpdateDrawSunflower(p, screenPos);
                        break;
                    case PT_COCONUT:
                        {
                            Vector2 origin = {(float)coconutSprite.width/2, coconutSprite.height-4};
                            PushDrawData(coconutSprite, LAYER_PLANTS, screenPos, origin, WHITE, 1, 0);
                        }
                        break;
                    case PT_POTATOBOMB:
                        UpdateDrawPotatoBomb(p, (Vector2){x, y,}, screenPos);
                        break;
                    default:
                        continue;
                }
            }
        }
    }
}

void UpdateDrawPotatoBomb(Plant* p, Vector2 gridPos, Vector2 screenPos)
{
    if (TickCooldown(&p->cooldown, plantCooldownLUT[p->type])) {
        Zombie* zArr = (Zombie*)zombies.array;
        for (int i = 0; i < zombies.fixedSize; i++) {
            if (zArr[i].active) {
                Zombie* z = &zArr[i];
                if (fabs((int)z->gridPos.x - gridPos.x) < 2 && fabs((int)z->gridPos.y - gridPos.y) < 2) {
                    z->health = 0;
                }
            }
        }

        CreateParticleExplosion(screenPos, (Vector2){8, 8}, 7, 30, 48, DARKBROWN);

        p->health = 0;
        SetSoundPitch(explodeSound, GetRandomFloatValue(0.8f, 1));
        PlaySound(explodeSound);
    }

    float coolDownPercent = (p->cooldown / (float)plantCooldownLUT[p->type]);
    float scale = 1+coolDownPercent*0.5f;

    Vector2 origin = {(float)potatoSprite.width/2, potatoSprite.height-2};
    PushDrawData(potatoSprite, LAYER_PLANTS, screenPos, origin, WHITE, scale, 0);
}


void UpdateDrawSunflower(Plant* p, Vector2 screenPos)
{
    Vector2 sunSpawnPos = Vector2Subtract(screenPos, (Vector2){0, 40});

    if (TickCooldown(&p->cooldown, plantCooldownLUT[p->type]))
        SpawnSun(sunSpawnPos);

    Vector2 origin = {sunflowerSprite.width/2, sunflowerSprite.height-4};

    float coolDownPercent = (p->cooldown / (float)plantCooldownLUT[p->type]);
    float scale = 1;
    if (coolDownPercent < 0.02f) {
        scale = 1+(coolDownPercent*4);
    }
    
    PushDrawData(sunflowerSprite, LAYER_PLANTS, screenPos, origin, WHITE, scale, 0);
}


void UpdateDrawSeedShooter(Plant* p, Vector2 gridPos, Vector2 screenPos)
{
    // Look for a zombie in our row
    Zombie* zArr = (Zombie*)zombies.array;
    bool zombieInRow = false;
    for (int i = 0; i < zombies.fixedSize; i++) {
        if (zArr[i].active && zArr[i].gridPos.y == gridPos.y && zArr[i].gridPos.x >= gridPos.x && zArr[i].gridPos.x <= GRID_WIDTH+0.75f) {
            zombieInRow = true;
            break;
        }
    }

    // Shoot pea if a zombie is in our row
    if (zombieInRow) {
        if (TickCooldown(&p->cooldown, plantCooldownLUT[p->type])) {
            Vector2 peaSpawnPos = Vector2Add(screenPos, (Vector2){18, -42});

            Projectile* proj = GetNextObject(projectiles, Projectile);
            IncrementArrayIndex(&projectiles);

            proj->active = true;
            proj->pos = peaSpawnPos;

            PlaySound(peaShootSound);
            CreateParticleExplosion(peaSpawnPos, (Vector2){2, 2}, 3, 30, 8, (Color){200, 255, 200, 255});
        }
    }

    float coolDownPercent = (p->cooldown / (float)plantCooldownLUT[p->type]);
    float scale = 1;
    if (coolDownPercent < 0.05f) {
        scale = 1+(coolDownPercent*3);
    }

    Vector2 origin = {seedShooterSprite.width/2, seedShooterSprite.height-4};
    PushDrawData(seedShooterSprite, LAYER_PLANTS, screenPos, origin, WHITE, scale, 0);
}
