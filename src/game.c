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
#include "zombie.h"

void UpdateDrawProjectiles(void);


int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;


    InitWindow(screenWidth, screenHeight, "Plants Vs Zombies Clone");

    InitAudioDevice();

    SetTargetFPS(60);

    LoadAssets();
    CreateSeedPackets();

    SetMusicVolume(themeSong, 0.5f);
    PlayMusicStream(themeSong);

    int frameCount = 0;
    bool waveStarted = false;

    bool playingMusic = true;

    while (!WindowShouldClose()) {

        if (playingMusic) {
            UpdateMusicStream(themeSong);
        }

        if (IsKeyPressed(KEY_M)) {
            playingMusic = !playingMusic;
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

        UpdateDrawPlants();



        // Draw top bar
        DrawRectangle(0, 0, screenWidth, SEEDPACKET_SIZE.y+20, DARKBROWN);
        
        // Draw seed tray
        int margin = 4;
        Vector2 trayStart = {seedPackets[0].origin.x-margin, seedPackets[0].origin.y-margin};
        Vector2 trayEnd = {trayStart.x+8*SEEDPACKET_SIZE.x+margin, seedPackets[0].origin.y+SEEDPACKET_SIZE.y};
        DrawRectangleV(trayStart, trayEnd, (Color){46, 40, 34, 255});

        UpdateDrawProjectiles();
        UpdateDrawParticles();
        UpdateDrawSeedPackets();
        UpdateDrawZombies();

        
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

    UnloadAssets();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void UpdateDrawProjectiles(void)
{
    // Collision behaviour is done inside zombie update
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
}
