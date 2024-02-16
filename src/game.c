#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "globals.h"
#include "assets.h"
#include "particles.h"
#include "plant.h"
#include "seed_packets.h"


typedef struct {
    Vector2 pos;
    bool active;
} Projectile;

// Projectile globals
#define MAX_PROJ 16
Projectile projectiles[MAX_PROJ] = {0};
int nextProjectile = 0;


void UpdateDrawPShooter(Plant* p, Vector2 gridPos, Vector2 screenPos);
void UpdateDrawSunflower(Plant* p, Vector2 screenPos);
void UpdateDrawCherryBomb(Plant* p, Vector2 gridPos, Vector2 screenPos);
void SpawnSun(Vector2 pos);
void DrawTextureCentered(Texture2D sprite, Vector2 pos, Vector2 origin, Color tint);


int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "Plants Vs Zombies Clone");

    InitAudioDevice();

    SetTargetFPS(60);

    LoadAssets();
    CreateSeedPackets();

    PlayMusicStream(themeSong);

    int fps = 60;
    int frameCount = 0;
    bool waveStarted = false;

    while (!WindowShouldClose()) {

        UpdateMusicStream(themeSong);

        if (IsKeyPressed(KEY_UP)) {
            fps += 60;
            SetTargetFPS(fps);
        }
        else if (IsKeyPressed(KEY_DOWN)) {
            fps = 60;
            SetTargetFPS(fps);
        }

        // Control zombie spawn rate during waves
        int seconds = frameCount/60;
        switch (seconds) {
            case 120:
                if (!waveStarted) {
                    waveStarted = true;
                    currentZombieSpawnRate = 60*3;
                    zombieSpawnCooldown = 0;
                }
                break;
            case 120+30:
                currentZombieSpawnRate = 60*20;
                waveStarted = false;
                break;

            case 240:
                if (!waveStarted) {
                    waveStarted = true;
                    currentZombieSpawnRate = 60*2;
                    zombieSpawnCooldown = 0;
                }
                break;
            case 240+30:
                currentZombieSpawnRate = 60*20;
                waveStarted = false;
                break;
        }


        BeginDrawing();

        ClearBackground(WHITE);

        // Draw background
        int tilesX = screenWidth/32;
        int tilesY = screenHeight/32;

        for (int x = 0; x < tilesX; x++) {
            for (int y = 0; y < tilesY; y++) {
                DrawTexture(lawnBackgroundSprite, x * 32, y * 32, WHITE);
            }
        }

        // Draw lawn grid
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {

                // Checkerboard tint
                Color c = {230, 255, 230, 50};
                bool xEven = (x % 2) == 0;
                bool yEven = (y % 2) == 0;
                if ((xEven && yEven) || (!xEven && !yEven)) {
                    c = (Color){230, 255, 230, 20};
                }

                Vector2 gridCellPos = Vector2Add(gridDrawOffset, (Vector2){x*gridCellSize.x, y*gridCellSize.y});

                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && draggingSeedPacket) {
                    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){gridCellPos.x, gridCellPos.y, gridCellSize.x, gridCellSize.y})) {
                        c = (Color){255, 255, 255, 100};
                    }
                }
                DrawRectangleV(gridCellPos, gridCellSize, c);
            }
        }

        // Update and Draw plants
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                if (gardenGrid[x][y].type != PT_NONE) {
                    // TODO: get rid of gridCellGap, I'm not using it as far as I can tell.
                    Vector2 screenPos = Vector2Add(gridDrawOffset, (Vector2){x*gridCellSize.x+gridCellSize.x/2, y*gridCellSize.y+gridCellSize.y*0.75f});
                    Plant* p = &gardenGrid[x][y];

                    if (p->health <= 0) {
                        p->type = PT_NONE;
                    }
                    

                    DrawTextureCentered(shadowSprite, screenPos, (Vector2){(float)shadowSprite.width/2, (float)shadowSprite.height/2}, WHITE);

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

        // Update and draw Projectiles
        for (int i = 0; i < MAX_PROJ; i++) {
            if (projectiles[i].active) {
                projectiles[i].pos.x += 5;
                DrawTextureCentered(smallShadowSprite, Vector2Add(projectiles[i].pos, (Vector2){0, 42}), Vector2Zero(), WHITE);
                DrawTextureV(peaSprite, projectiles[i].pos, WHITE);

                if (projectiles[i].pos.x > 640) {
                    projectiles[i].active = false;
                }
            }
        }


        // Draw top bar
        DrawRectangle(0, 0, screenWidth, SEEDPACKET_SIZE.y+20, DARKBROWN);
        
        // Draw seed tray
        int margin = 4;
        Vector2 trayStart = {seedPackets[0].origin.x-margin, seedPackets[0].origin.y-margin};
        Vector2 trayEnd = {trayStart.x+8*SEEDPACKET_SIZE.x+margin, seedPackets[0].origin.y+SEEDPACKET_SIZE.y};
        DrawRectangleV(trayStart, trayEnd, (Color){46, 40, 34, 255});

        // Spawn zombies
        zombieSpawnCooldown--;
        if (zombieSpawnCooldown <= 0) {
            zombieSpawnCooldown = currentZombieSpawnRate;
            float xSpawn = GetRandomValue(12, 14) + rand()/(float)RAND_MAX;
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

        zombieGrowlCooldown--;
        // Update and draw zombies
        for (int i = 0; i < MAX_ZOMBIES; i++) {
            if (zombies[i].active) {
                if (zombieGrowlCooldown < 0) {
                    PlaySound(zombieGrowlSounds[GetRandomValue(0, ZOMBIE_GROWL_SOUND_COUNT-1)]);
                    zombieGrowlCooldown = GetRandomValue(60*10, 60*15);
                }


                if (zombies[i].gridPos.x > 0 && zombies[i].gridPos.x < GRID_WIDTH) {
                    int frontOfZombieRounded = (int)roundf(zombies[i].gridPos.x)-1;
                    if (gardenGrid[frontOfZombieRounded][(int)zombies[i].gridPos.y].type != PT_NONE) {
                        gardenGrid[frontOfZombieRounded][(int)zombies[i].gridPos.y].health -= 0.5f;
                        // TODO: Play eating sounds
                    } else {
                        // TODO: get rid of this duplicate code. Either with a constant or by restructuring these statements.
                        zombies[i].gridPos.x -= 0.002f;
                    }

                } else {
                    zombies[i].gridPos.x -= 0.002f;
                }


                int x = gridDrawOffset.x + zombies[i].gridPos.x*gridCellSize.x;
                int y = gridDrawOffset.y + zombies[i].gridPos.y*gridCellSize.y;
                Rectangle box = {x-8, y-16, (float)zombieSprite.width/2, zombieSprite.height};

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

                // TODO: Draw shadow

                Rectangle src = {0,0,zombieSprite.width, zombieSprite.height};
                Rectangle dst = {x-(float)zombieSprite.width/2, y-16, zombieSprite.width, zombieSprite.height};
                DrawTexturePro(zombieSprite, src, dst, Vector2Zero(), 0, WHITE);

                // Zombie collider debug
                //DrawRectangleRec(box, (Color){100, 100, 255, 100});
            }
        }

        UpdateDrawParticles();


        UpdateSeedPackets();
        DrawSeedPackets();

        // Spawn suns
        sunCooldown--;
        if (sunCooldown <= 0) {
            sunCooldown = SUN_SPAWN_TIME;
            Vector2 pos = {GetRandomValue(64, screenWidth-64), SEEDPACKET_SIZE.y+32};
            SpawnSun(pos);
        }

        // Update and draw Suns
        for (int i = 0; i < MAX_SUNS; i++) {
            if (suns[i].active) {
                suns[i].pos.y += 0.25f;

                const int sunSize = sunSprite.width;
                Vector2 sunHalfSize = {(float)sunSize/2, (float)sunSize/2};

                // Handle sun being clicked on
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    Rectangle sunBox = {EXPAND_V2(Vector2Subtract(suns[i].pos, sunHalfSize)), sunSize, sunSize};
                    if (CheckCollisionPointRec(GetMousePosition(), sunBox)) {
                        suns[i].active = false;
                        sunsCollectedCount += SUN_VALUE;
                    }
                }

                // Draw sun
                Rectangle src = {0,0,sunSize, sunSize};
                Rectangle dst = { EXPAND_V2(suns[i].pos), sunSize, sunSize};
                DrawTexturePro(sunSprite, src, dst, sunHalfSize, GetTime()*10, WHITE);
            }
        }

        // Draw sun count
        char sunCountText[32];
        sprintf(sunCountText, "Sun:\n\n%i", sunsCollectedCount);
        DrawText(sunCountText, 32, 10, 20, WHITE);


        // Draw timer
        frameCount++;
        char timerText[32];
        sprintf(timerText, "%i", frameCount/60);
        DrawText(timerText, screenWidth-64, 10, 20, WHITE);

        EndDrawing();

    }

    // TODO: Properly unload assets

    CloseWindow();

    return 0;
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

void UpdateDrawCherryBomb(Plant* p, Vector2 gridPos, Vector2 screenPos)
{
    p->cooldown--;
    if (p->cooldown <= 0) {

        for (int i = 0; i < MAX_ZOMBIES; i++) {
            if (zombies[i].active) {
                Zombie* z = &zombies[i];
                if (fabs((int)z->gridPos.x - gridPos.x) < 2 && fabs((int)z->gridPos.y - gridPos.y) < 2) {
                    z->health = 0;
                }
            }
        }

        CreateParticleExplosion(screenPos, (Vector2){8, 8}, 5, 30, 48, DARKBROWN);

        p->health = 0;
    }

    Vector2 origin = {(float)wallnutSprite.width/2, wallnutSprite.height-4};
    DrawTextureCentered(wallnutSprite, screenPos, origin, WHITE);
}

void DrawTextureCentered(Texture2D sprite, Vector2 pos, Vector2 origin, Color tint)
{
    Rectangle src = {0, 0, sprite.width, sprite.height};
    Rectangle dst = {EXPAND_V2(pos), sprite.width, sprite.height};
    DrawTexturePro(sprite, src, dst, origin, 0, tint);
}

void UpdateDrawSunflower(Plant* p, Vector2 screenPos)
{
    Vector2 sunSpawnPos = Vector2Subtract(screenPos, (Vector2){0, 18});
    p->cooldown--;
    if (p->cooldown <= 0) {
        p->cooldown = plantCooldownLUT[PT_SUNFLOWER];
        SpawnSun(sunSpawnPos);
    }

    Vector2 origin = {(float)sunflowerSprite.width/2, sunflowerSprite.height-4};
    DrawTextureCentered(sunflowerSprite, screenPos, origin, WHITE);
}

void UpdateDrawPShooter(Plant* p, Vector2 gridPos, Vector2 screenPos)
{
    // Look for a zombie in our row
    bool zombieInRow = false;
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        if (zombies[i].active && zombies[i].gridPos.y == gridPos.y && zombies[i].gridPos.x >= gridPos.x && zombies[i].gridPos.x <= GRID_WIDTH+2) {
            zombieInRow = true;
            break;
        }
    }

    // Shoot pea if a zombie is in our row
    if (zombieInRow) {
        p->cooldown--;
        if (p->cooldown <= 0) {
            p->cooldown = plantCooldownLUT[PT_PSHOOTER];
            if (nextProjectile == MAX_PROJ) {
                nextProjectile = 0;


            }

            Vector2 peaSpawnPos = Vector2Add(screenPos, (Vector2){18, -42});
            projectiles[nextProjectile].active = true;
            projectiles[nextProjectile].pos = peaSpawnPos;
            nextProjectile++;

            PlaySound(peaShootSound);
            CreateParticleExplosion(peaSpawnPos, (Vector2){2, 2}, 3, 15, 8, (Color){200, 255, 200, 255});
        }
    }

    Vector2 origin = {(float)pShooterSprite.width/2, pShooterSprite.height-4};
    DrawTextureCentered(pShooterSprite, screenPos, origin, WHITE);
}
