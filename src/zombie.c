#include "zombie.h"
#include <raylib.h>
#include <stdlib.h>
#include "assets.h"
#include "globals.h"
#include <raymath.h>
#include "particles.h"

#define ZOMBIE_DEBUG false

Zombie zombies[MAX_ZOMBIES] = {0};
int nextZombie = 0;

int currentZombieSpawnRate;

int zombieSpawnCooldown;
int zombieSpawnCooldown;

int zombieGrowlCooldown;
int lastZombieGrowlIndex = -1;

void InitZombies(void)
{
    // Init zombies array
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        zombies[i].active = false;
    }
    nextZombie = 0;

    currentZombieSpawnRate = 60*20;

#if ZOMBIE_DEBUG
    zombieSpawnCooldown = 0;
#else
    zombieSpawnCooldown = 60*30;
#endif

    zombieGrowlCooldown = 60*2;
}

void UpdateDrawZombies(void)
{
    // Spawn zombies
    zombieSpawnCooldown--;
    if (zombieSpawnCooldown <= 0) {
        zombieSpawnCooldown = currentZombieSpawnRate;
#if ZOMBIE_DEBUG
        float xSpawn = 8;
#else
        float xSpawn = GetRandomValue(12, 14) + rand()/(float)RAND_MAX;
#endif
        // TODO: use constant for getrandom value max
        Vector2 gridPos = {xSpawn, GetRandomValue(0, 4)};

        // TODO: This is a duplicate of the sun spawning code. Consider refactoring.
        if (nextZombie == MAX_ZOMBIES) {
            nextZombie = 0;
        }

        zombies[nextZombie].active = true;
        zombies[nextZombie].health = 1.0f;
        zombies[nextZombie].gridPos = gridPos;

        nextZombie++;
    }

    // Update and draw zombies
    zombieGrowlCooldown--;
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        Zombie* zombie = &zombies[i];
        if (zombie->active) {
            // Growl
            if (zombieGrowlCooldown < 0) {
                int random;
                do {
                    random = GetRandomValue(0, ZOMBIE_GROWL_SOUND_COUNT-1);
                } while (random == lastZombieGrowlIndex);
                lastZombieGrowlIndex = random;
                // TODO: Randomise pitch. Will need to make a good random float function
                PlaySound(zombieGrowlSounds[random]);
                // One in five chance that the cooldown will be shorter, else, it should be longer
                if (GetRandomValue(0, 4)) {
                    zombieGrowlCooldown = GetRandomValue(30, 60*5);
                } else {
                    zombieGrowlCooldown = GetRandomValue(60*5, 60*15);
                }
            }

            // Move and eat plants
            float moveAmount = 0.002f;

            if (zombies[i].gridPos.x > 0 && zombies[i].gridPos.x < GRID_WIDTH) {
                int frontOfZombieRounded = (int)roundf(zombies[i].gridPos.x)-1;
                Plant* p = &gardenGrid[frontOfZombieRounded][(int)zombies[i].gridPos.y];

                if (p->type != PT_NONE) {
                    p->health -= 0.5f;
                    moveAmount = 0;
                    // TODO: Play eating sounds
                }
            }

            zombies[i].gridPos.x -= moveAmount;


            int x = gridDrawOffset.x + zombies[i].gridPos.x*gridCellSize.x;
            int y = gridDrawOffset.y + zombies[i].gridPos.y*gridCellSize.y;
            Rectangle box = {x-16, y-16, (float)zombieSprite.width/2, zombieSprite.height};

            // Handle collisions with projectiles
            for (int j = 0; j < MAX_PROJ; j++) {
                if (projectiles[j].active) {
                    if (CheckCollisionPointRec(projectiles[j].pos, box)) {
                        projectiles[j].active = false;
                        zombies[i].health -= 0.1f;
                        PlaySound(popSound);
                        PlaySound(zombieHitSounds[GetRandomValue(0, ZOMBIE_HIT_SOUND_COUNT-1)]);
                        CreateParticleExplosion(projectiles[j].pos, (Vector2){3,3}, 3, 15, 16, (Color){100, 0, 0, 255});
                    }
                } 
            }

            if (zombies[i].health <= 0) {
                zombies[i].active = false;
            }

            // Draw
            Vector2 drawPos = {x, y+gridCellSize.y*0.75f};
            Vector2 origin = {(float)zombieSprite.width*0.75f, zombieSprite.height-4};
            DrawTextureCentered(shadowSprite, drawPos, SHADOW_ORIGIN, WHITE);
            DrawTextureCentered(zombieSprite, drawPos, origin, WHITE);

            // Zombie collider debug
            //DrawRectangleRec(box, (Color){100, 100, 255, 100});
        }
    }
}
