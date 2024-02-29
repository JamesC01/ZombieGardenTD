#include "zombie.h"
#include <math.h>
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
FixedObjectArray zombieHeads;

// TODO: Figure out if there's a way to bundle up these globals
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

    defaultZombieSpawnRate = 60 * 20;
    defaultWaveZombieSpawnRate = 60 * 3;
    currentZombieSpawnRate = defaultZombieSpawnRate;

#if ZOMBIE_DEBUG
    zombieSpawnTimer = 0;
#else
    zombieSpawnTimer = 60*30;
#endif

    zombieGrowlTimer = 60*2;
}

void UpdateDrawZombieHeads(void)
{
    ZombieHead* heads = (ZombieHead*)zombieHeads.array;
    for (int i = 0; i < zombieHeads.fixedSize; i++) {
        ZombieHead* head = &heads[i];

        if (head->active) {
            float gravity = 6 * GetFrameTime();

            if (head->pos.y >= head->floorY) {
                head->pos.y = head->floorY;
            } else {
                head->velocity.y += gravity;
            }

            head->pos.x += head->velocity.x;
            head->pos.y += head->velocity.y;
            head->rotation += head->angularVel;


            if (head->pos.x > virtualScreenWidth) {
                head->active = false;
            }

            DrawTextureCentered(shadowSprite, (Vector2){head->pos.x, head->floorY+zombieHeadSprite.height/2.0f}, GetTextureCenterPoint(shadowSprite), WHITE, 1);

            Rectangle src = {0,0, zombieHeadSprite.width, zombieHeadSprite.height};
            Rectangle dst = {head->pos.x, head->pos.y, zombieHeadSprite.width, zombieHeadSprite.height};
            DrawTexturePro(zombieHeadSprite, src, dst, GetTextureCenterPoint(zombieHeadSprite), head->rotation, WHITE);
        }
    }
}

void UpdateDrawZombies(void)
{
    // Handle ending wave
    if (waveStarted && currentKillsRequiredToEndWave <= 0) {
        printf("Wave ended.\n");
        waveStarted = false;

        defaultZombieSpawnRate -= 60 * 3;
        if (defaultZombieSpawnRate < 60*2)
            defaultZombieSpawnRate = 60 * 2;

        defaultWaveZombieSpawnRate -= 30;
        if (defaultWaveZombieSpawnRate < 60)
            defaultWaveZombieSpawnRate = 60;

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
        float xSpawn = 7;
#else
        float xSpawn = GetRandomValue(10, 11) + rand()/(float)RAND_MAX;
#endif
        float ySpawn = GetUniqueRandomValue(lastZombieSpawnYIndex, 0, GRID_HEIGHT-1);
        lastZombieSpawnYIndex = ySpawn;
        Vector2 gridPos = {xSpawn, ySpawn};

        Zombie* zombie = GetNextObject(zombies, Zombie);
        IncrementArrayIndex(&zombies);

        zombie->active = true;
        zombie->health = 1.0f;
        zombie->gridPos = gridPos;
        zombie->headless = false;
    }

    // Update and draw zombies
    Zombie* zArr = (Zombie*)zombies.array;
    zombieGrowlTimer--;
    for (int i = 0; i < zombies.fixedSize; i++) {
        Zombie* zombie = &zArr[i];

        if (zombie->active) {

            zombie->scale = 1+((1+sinf(GetTime()*4+zombie->gridPos.x*10))/2)*0.075f;

            // Growl
            if (zombieGrowlTimer < 0) {
                int random = GetUniqueRandomValue(lastZombieGrowlIndex, 0, ZOMBIE_GROWL_SOUND_COUNT-1);
                lastZombieGrowlIndex = random;

                SetSoundPitch(zombieGrowlSounds[random], GetRandomFloatValue(0.9f, 1.1f));
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
                        if (!zombie->headless) {
                            p->health -= 0.5f;
                        }
                        moveAmount = 0;
                        // TODO: Play eating sounds
                    }
                }
            }

            zombie->gridPos.x -= moveAmount;

            if (zombie->gridPos.x <= -1 && !zombie->headless) {
                GameOver();
            }


            int sX = gridDrawOffset.x + zombie->gridPos.x*gridCellSize.x;
            int sY = gridDrawOffset.y + zombie->gridPos.y*gridCellSize.y;
            Rectangle bounds = {sX-16, sY-16, (float)zombieSprite.width/2, zombieSprite.height};

            // Handle collisions with projectiles
            Projectile* projArr = (Projectile*)projectiles.array;
            for (int j = 0; j < projectiles.fixedSize; j++) {
                if (projArr[j].active) {
                    if (CheckCollisionPointRec(projArr[j].pos, bounds)) {
                        projArr[j].active = false;
                        zombie->health -= 0.1f;
                        SetSoundPitch(popSound, GetRandomFloatValue(0.95f, 1.05f));
                        PlaySound(popSound);
                        PlaySound(zombieHitSounds[GetRandomValue(0, ZOMBIE_HIT_SOUND_COUNT-1)]);
                        CreateParticleExplosion(projArr[j].pos, (Vector2){3,3}, 3, 30, 16, (Color){100, 0, 0, 255});
                        if (GetRandomValue(0, 49) == 0) {
                            CreateParticleConfetti(projArr[j].pos, (Vector2){3,3}, 64);
                        }
                    }
                } 
            }

            Vector2 origin = {(float)zombieSprite.width*0.75f, zombieSprite.height-4};

            if (zombie->health <= 0 && !zombie->headless) {
                zombie->headless = true;
                zombie->headlessTimer = 60 * 3;
                ZombieKilled();

                ZombieHead* head = GetNextObject(zombieHeads, ZombieHead);
                IncrementArrayIndex(&zombieHeads);

                head->floorY = sY + gridCellSize.y*0.5f;
                head->angularVel = GetRandomFloatValue(5, 10);
                head->velocity = (Vector2){1, GetRandomFloatValue(-6, -1)};
                head->pos = Vector2Add((Vector2){sX, sY}, (Vector2){10, 0});
                head->active = true;
            }

            
            if (zombie->headless) {
                zombie->headlessTimer--;
                if (zombie->headlessTimer <= 0) {
                    zombie->active = false;
                    // TODO: Consider making some sort of death animation.
                }
            }

            Texture sprite = (zombie->headless) ? headlessZombieSprite : zombieSprite;

            // Draw
            Vector2 drawPos = {sX, sY+gridCellSize.y*0.75f};
            DrawTextureCentered(shadowSprite, drawPos, GetTextureCenterPoint(shadowSprite), WHITE, 1);
            DrawTextureCentered(sprite, drawPos, origin, WHITE, zombie->scale);

#if ZOMBIE_DEBUG
            // Zombie collider debug
            DrawRectangleRec(bounds, (Color){100, 100, 255, 100});
#endif
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
