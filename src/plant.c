#include "plant.h"
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


                DrawTextureCentered(shadowSprite, screenPos, SHADOW_ORIGIN, WHITE);

                switch (p->type) {
                    case PT_PSHOOTER:
                        UpdateDrawPShooter(p, (Vector2){x, y}, screenPos);
                        break;
                    case PT_SUNFLOWER:
                        UpdateDrawSunflower(p, screenPos);
                        break;
                    case PT_WALLNUT:
                        {
                            Vector2 origin = {(float)wallnutSprite.width/2, wallnutSprite.height-4};
                            DrawTextureCentered(wallnutSprite, screenPos, origin, WHITE);
                        }
                        break;
                    case PT_CHERRYBOMB:
                        UpdateDrawCherryBomb(p, (Vector2){x, y,}, screenPos);
                        break;
                    default:
                        continue;
                }
            }
        }
    }
}

void UpdateDrawCherryBomb(Plant* p, Vector2 gridPos, Vector2 screenPos)
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
    }

    Vector2 origin = {(float)cherrySprite.width/2, cherrySprite.height-2};
    DrawTextureCentered(cherrySprite, screenPos, origin, WHITE);
}


void UpdateDrawSunflower(Plant* p, Vector2 screenPos)
{
    Vector2 sunSpawnPos = Vector2Subtract(screenPos, (Vector2){0, 18});

    if (TickCooldown(&p->cooldown, plantCooldownLUT[p->type]))
        SpawnSun(sunSpawnPos);

    Vector2 origin = {(float)sunflowerSprite.width/2, sunflowerSprite.height-4};
    DrawTextureCentered(sunflowerSprite, screenPos, origin, WHITE);
}


void UpdateDrawPShooter(Plant* p, Vector2 gridPos, Vector2 screenPos)
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

            Projectile* projArr = (Projectile*)projectiles.array;
            projArr[projectiles.next].active = true;
            projArr[projectiles.next].pos = peaSpawnPos;

            NextObject(&projectiles);

            PlaySound(peaShootSound);
            CreateParticleExplosion(peaSpawnPos, (Vector2){2, 2}, 3, 30, 8, (Color){200, 255, 200, 255});
        }
    }

    Vector2 origin = {(float)pShooterSprite.width/2, pShooterSprite.height-4};
    DrawTextureCentered(pShooterSprite, screenPos, origin, WHITE);
}
