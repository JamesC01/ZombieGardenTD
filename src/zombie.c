#include "zombie.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "assets.h"
#include "globals.h"
#include <raymath.h>
#include "particles.h"
#include "game.h"

#define ZOMBIE_DEBUG true

Zombie zombies[MAX_ZOMBIES] = {0};
int nextZombie = 0;

const int defaultZombieSpawnRate = 60 * 20;

int currentZombieSpawnRate;
int zombieSpawnTimer;

int zombieGrowlTimer;
int lastZombieGrowlIndex = -1;

int lastZombieSpawnYIndex = -1;

// Wave variables
bool waveStarted;
int waveTimer;
int killsRequiredForNextWave;
int currentKillsRequiredForNextWave;

int zombiesKilledCount;


void ZombieKilled(void);

void InitZombies(void)
{
    waveTimer = 0;
    waveStarted = false;
    killsRequiredForNextWave = 5;
    currentKillsRequiredForNextWave = killsRequiredForNextWave;
    zombiesKilledCount = 0;

    // Init zombies array
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        zombies[i].active = false;
    }
    nextZombie = 0;

    currentZombieSpawnRate = defaultZombieSpawnRate;

#if ZOMBIE_DEBUG
    zombieSpawnTimer = 0;
#else
    zombieSpawnTimer = 60*30;
#endif

    zombieGrowlTimer = 60*2;
}

void UpdateDrawZombies(void)
{
    // Tick wave timer
    waveTimer--;
    if (waveTimer <= 0) {
        waveStarted = false;
        currentZombieSpawnRate = defaultZombieSpawnRate;
    }

    // Spawn zombies
    zombieSpawnTimer--;
    if (zombieSpawnTimer <= 0) {
        zombieSpawnTimer = currentZombieSpawnRate;

#if ZOMBIE_DEBUG
        float xSpawn = 2;
#else
        float xSpawn = GetRandomValue(10, 11) + rand()/(float)RAND_MAX;
#endif
        // TODO: use constant for getrandom value max
        float ySpawn = GetUniqueRandomValue(lastZombieSpawnYIndex, 0, 4);
        lastZombieSpawnYIndex = ySpawn;
        Vector2 gridPos = {xSpawn, ySpawn};

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
    zombieGrowlTimer--;
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        Zombie* zombie = &zombies[i];
        if (zombie->active) {
            // Growl
            if (zombieGrowlTimer < 0) {
                int random = GetUniqueRandomValue(lastZombieGrowlIndex, 0, ZOMBIE_GROWL_SOUND_COUNT-1);
                lastZombieGrowlIndex = random;
                // TODO: Randomise pitch. Will need to make a good random float function
                PlaySound(zombieGrowlSounds[random]);
                // One in five chance that the cooldown will be shorter, else, it should be longer
                if (GetRandomValue(0, 4)) {
                    zombieGrowlTimer = GetRandomValue(30, 60*5);
                } else {
                    zombieGrowlTimer = GetRandomValue(60*5, 60*15);
                }
            }

            // Move and eat plants
            float moveAmount = 0.002f;

            if (zombies[i].gridPos.x > 0 && zombies[i].gridPos.x < GRID_WIDTH) {
                int frontOfZombieRounded = (int)roundf(zombies[i].gridPos.x)-1;
                if (frontOfZombieRounded >= 0) {
                    Plant* p = &gardenGrid[frontOfZombieRounded][(int)zombies[i].gridPos.y];

                    if (p->type != PT_NONE) {
                        p->health -= 0.5f;
                        moveAmount = 0;
                        // TODO: Play eating sounds
                    }
                }
            }

            zombies[i].gridPos.x -= moveAmount;

            if (zombies[i].gridPos.x <= -1) {
                GameOver();
            }


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
                ZombieKilled();
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

void ZombieKilled(void)
{
    zombiesKilledCount++;

    if (!waveStarted) {
        currentKillsRequiredForNextWave--;

        // Start wave
        if (currentKillsRequiredForNextWave <= 0) {
            printf("Wave started!\n");
            waveStarted = true;

            zombieSpawnTimer = 0;
            // TODO: Consider varying wave size as game goes on
            currentZombieSpawnRate = 60 * 3;
            waveTimer = 60 * 30;

            int zombiesThisWave = waveTimer/currentZombieSpawnRate;

            // You have to kill *all* the zombies this wave, plus the ever increasing count required for the next wave
            // TODO: consider changing this to make it easier to understand
            killsRequiredForNextWave += 5;
            currentKillsRequiredForNextWave = zombiesThisWave + killsRequiredForNextWave;

        }
    }

}
