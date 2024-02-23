#include "zombie.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "assets.h"
#include "game.h"
#include <raymath.h>
#include "particles.h"
#include "game.h"

#define ZOMBIE_DEBUG false

FixedObjectArray zombies;

int defaultZombieSpawnRate;
int defaultWaveZombieSpawnRate;

float zombieMoveSpeed;

int currentZombieSpawnRate;
int zombieSpawnTimer;

int zombieGrowlTimer;
int lastZombieGrowlIndex = -1;

int lastZombieSpawnYIndex = -1;

// Wave variables
bool waveStarted;
int fastSpawningTimer; // How long should zombies spawn quickly during a wave
const int killsRequiredForNextWave = 5;
int currentKillsRequiredForNextWave;

int killsRequiredToEndWave;
int currentKillsRequiredToEndWave;

int zombiesKilledCount;


void ZombieKilled(void);

void InitZombies(void)
{
    zombieMoveSpeed = 0.002f;
    waveStarted = false;
    currentKillsRequiredForNextWave = killsRequiredForNextWave;
    zombiesKilledCount = 0;

    killsRequiredToEndWave = 10;
    currentKillsRequiredToEndWave = 0;
    // Init zombies array
    Zombie *zomArr = (Zombie*)zombies.array;
    for (int i = 0; i < zombies.fixedSize; i++) {
        zomArr[i].active = false;
    }

    currentZombieSpawnRate = defaultZombieSpawnRate;

#if ZOMBIE_DEBUG
    zombieSpawnTimer = 0;
#else
    zombieSpawnTimer = 60*30;
#endif

    zombieGrowlTimer = 60*2;

    defaultZombieSpawnRate = 60 * 20;
    defaultWaveZombieSpawnRate = 60 * 3;
}

void UpdateDrawZombies(void)
{
    // Handle ending wave
    if (waveStarted && currentKillsRequiredToEndWave <= 0) {
        printf("Wave ended.\n");
        waveStarted = false;
        defaultZombieSpawnRate -= 60 * 3;
        defaultWaveZombieSpawnRate -= 30;
        currentZombieSpawnRate = defaultZombieSpawnRate;
        zombieSpawnTimer = 0;
        zombieMoveSpeed += 0.0005f;
    }

    if (waveStarted) {
        fastSpawningTimer--;
        if (fastSpawningTimer <= 0) {
            currentZombieSpawnRate = INT_MAX; // Stop zombies spawning once all in the wave have spawned
        }
    }

    // Spawn zombies
    if (TickCooldown(&zombieSpawnTimer, currentZombieSpawnRate)) {

#if ZOMBIE_DEBUG
        float xSpawn = 2;
#else
        float xSpawn = GetRandomValue(10, 11) + rand()/(float)RAND_MAX;
#endif
        // TODO: use constant for getrandom value max
        float ySpawn = GetUniqueRandomValue(lastZombieSpawnYIndex, 0, 4);
        lastZombieSpawnYIndex = ySpawn;
        Vector2 gridPos = {xSpawn, ySpawn};

        Zombie* zombie = GET_NEXT_OBJECT(zombies, Zombie);

        zombie->active = true;
        zombie->health = 1.0f;
        zombie->gridPos = gridPos;

        NextObject(&zombies);
    }

    // Update and draw zombies
    Zombie* zArr = (Zombie*)zombies.array;
    zombieGrowlTimer--;
    for (int i = 0; i < zombies.fixedSize; i++) {
        Zombie* zombie = &zArr[i];

        if (zombie->active) {
            // Growl
            if (zombieGrowlTimer < 0) {
                int random = GetUniqueRandomValue(lastZombieGrowlIndex, 0, ZOMBIE_GROWL_SOUND_COUNT-1);
                lastZombieGrowlIndex = random;
                // TODO: Randomise pitch. Will need to make a good random float function
                PlaySound(zombieGrowlSounds[random]);

                bool shortCooldown = GetRandomValue(0, 4) == 0;
                if (shortCooldown) {
                    zombieGrowlTimer = GetRandomValue(30, 60*5);
                } else {
                    zombieGrowlTimer = GetRandomValue(60*5, 60*15);
                }
            }

            // Move and eat plants
            float moveAmount = zombieMoveSpeed;

            if (zombie->gridPos.x > 0 && zombie->gridPos.x < GRID_WIDTH) {
                int frontOfZombieRounded = (int)roundf(zombie->gridPos.x)-1;
                if (frontOfZombieRounded >= 0) {
                    Plant* p = &gardenGrid[frontOfZombieRounded][(int)zombie->gridPos.y];

                    if (p->type != PT_NONE) {
                        p->health -= 0.5f;
                        moveAmount = 0;
                        // TODO: Play eating sounds
                    }
                }
            }

            zombie->gridPos.x -= moveAmount;

            if (zombie->gridPos.x <= -1) {
                GameOver();
            }


            int x = gridDrawOffset.x + zombie->gridPos.x*gridCellSize.x;
            int y = gridDrawOffset.y + zombie->gridPos.y*gridCellSize.y;
            Rectangle box = {x-16, y-16, (float)zombieSprite.width/2, zombieSprite.height};

            // Handle collisions with projectiles
            Projectile* projArr = (Projectile*)projectiles.array;
            for (int j = 0; j < projectiles.fixedSize; j++) {
                if (projArr[j].active) {
                    if (CheckCollisionPointRec(projArr[j].pos, box)) {
                        projArr[j].active = false;
                        zombie->health -= 0.1f;
                        PlaySound(popSound);
                        PlaySound(zombieHitSounds[GetRandomValue(0, ZOMBIE_HIT_SOUND_COUNT-1)]);
                        CreateParticleExplosion(projArr[j].pos, (Vector2){3,3}, 3, 15, 16, (Color){100, 0, 0, 255});
                    }
                } 
            }

            if (zombie->health <= 0) {
                zombie->active = false;
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

    // TODO: Consider only ended wave once an certain number of zombies is killed. This number can increase as
    // the waves go on. It would be a better wave of determining the end of a wave compared to a timer.
    if (!waveStarted) {
        currentKillsRequiredForNextWave--;

        // Start wave
        if (currentKillsRequiredForNextWave <= 0) {
            printf("Wave started!\n");
            waveStarted = true;

            zombieSpawnTimer = 0;
            currentZombieSpawnRate = defaultWaveZombieSpawnRate;

            fastSpawningTimer = currentZombieSpawnRate * killsRequiredToEndWave;

            currentKillsRequiredForNextWave = killsRequiredForNextWave;

            currentKillsRequiredToEndWave = killsRequiredToEndWave;
            killsRequiredToEndWave += 5;
        }
    } else {
        currentKillsRequiredToEndWave--;
    }

}
