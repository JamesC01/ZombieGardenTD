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
#include "seed_packets.h"

FixedObjectArray zombies;
FixedObjectArray zombieHeads;

bool debugZombieSpawning = false;

int defaultZombieSpawnRate;
int defaultWaveZombieSpawnRate;

float zombieMoveSpeed;

int currentZombieSpawnRate;
int zombieSpawnTimer;

int zombieGrowlTimer;
int lastZombieGrowlIndex = -1;

int lastZombieSpawnYIndex = -1;

typedef struct {
    bool started;
    int fastSpawningTimer;

    int killsRequiredForNextWave;
    int killsLeftForNextWave;

    int killsRequiredToEndWave;
    int killsLeftToEndWave;
} WaveState;

WaveState waveState;

int zombiesKilledCount;


void ZombieKilled(void);
void SpawnZombie(Vector2 gridPos);

void InitZombies(void)
{
    waveState.started = false;
    waveState.killsRequiredForNextWave = 5;
    waveState.killsLeftForNextWave = waveState.killsRequiredForNextWave;
    waveState.killsRequiredToEndWave = 10;
    waveState.killsLeftToEndWave = 0;

    zombiesKilledCount = 0;
    zombieMoveSpeed = 0.002f;

    // Init zombies array
    Zombie *zomArr = (Zombie*)zombies.array;
    for (int i = 0; i < zombies.fixedSize; i++) {
        zomArr[i].active = false;
    }

    defaultZombieSpawnRate = 60 * 20;
    defaultWaveZombieSpawnRate = 60 * 3;
    currentZombieSpawnRate = defaultZombieSpawnRate;

    zombieSpawnTimer = 60*30;

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
                if (!head->onGround) {
                    PlaySound(headSplatSound);
                    head->onGround = true;
                        CreateParticleExplosion(head->pos, (Vector2){3,3}, 3, 30, 16, (Color){100, 0, 0, 255});
                }
                head->pos.y = head->floorY;
            } else {
                head->velocity.y += gravity;
            }

            head->pos.x += head->velocity.x;
            head->pos.y += head->velocity.y;
            head->rotation += head->angularVel;


            if (head->pos.x > virtualScreenWidth+virtualScreenWidth*0.5f) {
                head->active = false;
            }

            Vector2 shadowPos = {head->pos.x, head->floorY+zombieHeadSprite.height/2.0f};
            PushDrawData(shadowSprite, LAYER_ZOMBIES+head->rowIndex*10-1, shadowPos, GetTextureCenterPoint(shadowSprite), WHITE, 1, 0);
            PushDrawData(zombieHeadSprite, LAYER_ZOMBIES+head->rowIndex*10, head->pos, GetTextureCenterPoint(zombieHeadSprite), WHITE, 1, head->rotation);
        }
    }
}

void SpawnZombie(Vector2 gridPos)
{
    Zombie* zombie = GetNextObject(zombies, Zombie);
    IncrementArrayIndex(&zombies);

    zombie->active = true;
    zombie->health = 1.0f;
    zombie->gridPos = gridPos;
    zombie->headless = false;
}

void UpdateDrawZombies(void)
{

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && debugZombieSpawning && !draggingSeedPacket) {
        Vector2 mPos = GetMousePosVirtual();
        Vector2 gridPos = {(mPos.x-gridDrawOffset.x)/gridCellSize.x, (mPos.y-gridDrawOffset.y)/gridCellSize.y};
        gridPos.x = roundf(gridPos.x);
        gridPos.y = roundf(gridPos.y);

        SpawnZombie(gridPos);
    }

    // Handle ending wave
    if (waveState.started && waveState.killsLeftToEndWave <= 0) {
        printf("Wave ended.\n");
        waveState.started = false;

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

    if (waveState.started) {
        waveState.fastSpawningTimer--;
        if (waveState.fastSpawningTimer <= 0) {
            currentZombieSpawnRate = INT_MAX; // Stop zombies spawning once all in the wave have spawned
        }
    }

    // Spawn zombies
    if (TickCooldown(&zombieSpawnTimer, currentZombieSpawnRate)) {
        float xSpawn = GetRandomValue(10, 11) + rand()/(float)RAND_MAX;
        float ySpawn = GetUniqueRandomValue(lastZombieSpawnYIndex, 0, GRID_HEIGHT-1);
        lastZombieSpawnYIndex = ySpawn;
        Vector2 gridPos = {xSpawn, ySpawn};

        SpawnZombie(gridPos);
    }

    // Update and draw zombies
    Zombie* zArr = (Zombie*)zombies.array;
    zombieGrowlTimer--;
    for (int i = 0; i < zombies.fixedSize; i++) {
        Zombie* zombie = &zArr[i];

        if (zombie->active) {

            // Complicated timings to make the zombie animations time decently
            zombie->scale = 1+((1+sinf(zombieMoveSpeed*600*GetTime()*4+zombie->gridPos.x*10+i*15))/2)*0.075f;
            zombie->rotation = sinf((zombieMoveSpeed*600*GetTime()*4+zombie->gridPos.x*10+i*15)/2)*3;

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
                        zombie->flashTimer = 4;
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

            Vector2 origin = {(float)zombieSprite.width*0.7f, zombieSprite.height-4};

            if (zombie->health <= 0 && !zombie->headless) {
                zombie->headless = true;
                zombie->headlessTimer = 60 * 3;
                ZombieKilled();

                ZombieHead* head = GetNextObject(zombieHeads, ZombieHead);
                IncrementArrayIndex(&zombieHeads);

                head->floorY = sY + gridCellSize.y*0.5f;
                head->rowIndex = zombie->gridPos.y;
                head->angularVel = GetRandomFloatValue(5, 10);
                head->velocity = (Vector2){1, GetRandomFloatValue(-6, -1)};
                head->pos = Vector2Add((Vector2){sX, sY}, (Vector2){10, 0});
                head->active = true;
                head->onGround = false;
            }

            
            if (zombie->headless) {
                zombie->headlessTimer--;
                if (zombie->headlessTimer <= 0) {
                    zombie->active = false;
                    // TODO: Consider making some sort of death animation.
                }
            }

            Texture sprite = (zombie->headless) ? headlessZombieSprite : zombieSprite;
            Texture flashSprite = (zombie->headless) ? headlessZombieFlashSprite : zombieFlashSprite;

            // Draw
            Vector2 drawPos = {sX, sY+gridCellSize.y*0.75f};

            // Fade the zombie once it's headless
            Color c = WHITE;
            if (zombie->headless) {
                float transparencyPercent = Clamp((float)zombie->headlessTimer*16 / (60 * 3), 0, 1);
                c = Fade(c, transparencyPercent);
            }


            PushDrawData(shadowSprite, LAYER_ZOMBIES+zombie->gridPos.y*10-1, drawPos, GetTextureCenterPoint(shadowSprite), WHITE, 1, 0);
            PushDrawData(sprite, LAYER_ZOMBIES+zombie->gridPos.y*10, drawPos, origin, c, zombie->scale, zombie->rotation);

            if (zombie->flashTimer > 0) {
                zombie->flashTimer--;
                PushDrawData(flashSprite, LAYER_ZOMBIES+zombie->gridPos.y*10+1, drawPos, origin, c, zombie->scale, zombie->rotation);
            }

            // TODO: Add debug colliders
        }
    }
}

void ZombieKilled(void)
{
    zombiesKilledCount++;

    if (!waveState.started) {
        waveState.killsLeftForNextWave--;

        // Start wave
        if (waveState.killsLeftForNextWave <= 0) {
            printf("Wave started!\n");
            waveState.started = true;

            zombieSpawnTimer = 0;
            currentZombieSpawnRate = defaultWaveZombieSpawnRate;

            waveState.fastSpawningTimer = currentZombieSpawnRate * waveState.killsRequiredToEndWave;

            waveState.killsLeftForNextWave = waveState.killsRequiredForNextWave;

            waveState.killsLeftToEndWave = waveState.killsRequiredToEndWave;
            waveState.killsRequiredToEndWave += 5;
        }
    } else {
        waveState.killsLeftToEndWave--;
    }

}
