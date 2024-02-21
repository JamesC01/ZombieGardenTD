#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "assets.h"
#include "particles.h"
#include "plant.h"
#include "seed_packets.h"
#include "zombie.h"
#include "game.h"
#include "sun.h"

typedef enum {
    GAME_SCREEN_START,
    GAME_SCREEN_PLAYING,
    GAME_SCREEN_GAMEOVER,
    GAME_SCREEN_EXIT
} GameScreen;

GameScreen currentScreen = GAME_SCREEN_START;

const int screenWidth = 640;
const int screenHeight = 480;

const int defaultFps = 60;
bool limitFrameRate = true;
bool paused = false;
int frameCount = 0;

Plant gardenGrid[GRID_WIDTH][GRID_HEIGHT] = {0};
Vector2 gridDrawOffset = {40, 80};
Vector2 gridCellSize = {65, 78};

Projectile projectiles[MAX_PROJ] = {0};
int nextProjectile = 0;

void UpdateDrawStart(void);
void UpdateDrawGame(void);
void UpdateDrawGameOver(void);
void UpdateDrawProjectiles(void);
void InitializeGame(void);
void DrawBackground(void);


int main(void)
{

    InitWindow(screenWidth, screenHeight, "Plants Vs Zombies Clone");

    InitAudioDevice();

    SetTargetFPS(defaultFps);
    SetExitKey(KEY_NULL);

    LoadAssets();

    SetMusicVolume(themeSong, 0.5f);
    PlayMusicStream(themeSong);

    InitializeGame();

    bool playingMusic = true; // TODO: should be true by default


    bool shouldClose = false;
    while (!WindowShouldClose() && !shouldClose) {

        // Toggle frame limiter
        if (IsKeyPressed(KEY_S)) {
            limitFrameRate = !limitFrameRate;
            if (limitFrameRate) {
                SetTargetFPS(defaultFps);
            } else {
                SetTargetFPS(0);
            }
        }

        if (IsKeyPressed(KEY_M)) {
            playingMusic = !playingMusic;
        }


        if (playingMusic) {
            UpdateMusicStream(themeSong);
        }


        BeginDrawing();

        ClearBackground(WHITE);
        switch (currentScreen) {
            case GAME_SCREEN_START:
                UpdateDrawStart();
                break;
            case GAME_SCREEN_PLAYING:
                UpdateDrawGame();
                break;
            case GAME_SCREEN_GAMEOVER:
                UpdateDrawGameOver();
                break;
            case GAME_SCREEN_EXIT:
                shouldClose = true;
                break;
        }

        // Draw FPS
        char fpsText[32];
        sprintf(fpsText, "%ifps", GetFPS());
        DrawText(fpsText, 16, screenHeight-30, 25, WHITE);

        EndDrawing();


    }

    UnloadAssets();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void UpdateDrawStart(void)
{
    DrawBackground();

    DrawText("Raylib\n\n\nPlants Vs Zombies\n\n\nClone", 16, 16, 50, WHITE);
    DrawText("Press Enter to Start Game", 16, screenHeight/2, 25, WHITE);
    DrawText("Press Q to Quit Game", 16, screenHeight/2+25, 25, WHITE);

    if (IsKeyPressed(KEY_ENTER)) {
        currentScreen = GAME_SCREEN_PLAYING;
        InitializeGame();
    } else if (IsKeyPressed(KEY_Q)) {
        currentScreen = GAME_SCREEN_EXIT;
    }
}

void UpdateDrawGameOver(void)
{
    DrawBackground();


    DrawText("YOU DIED!\n\n\nGAME OVER :(", 16, 16, 50, WHITE);

    char killCountText[32];
    sprintf(killCountText, "You killed %i zombies!", zombiesKilledCount);
    DrawText(killCountText, 16, screenHeight/2-25, 25, WHITE);

    DrawText("Press Enter to return to Start\n", 16, screenHeight/2, 25, WHITE);

    if (IsKeyPressed(KEY_ENTER)) {
        currentScreen = GAME_SCREEN_START;
    }
}

void UpdateDrawGame(void)
{
    if (IsKeyPressed(KEY_ESCAPE)) {
        currentScreen = GAME_SCREEN_START;
    }

    if (IsKeyPressed(KEY_P)) {
        paused = !paused;
    }

    if (paused)
        return;

    DrawBackground();

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

            if (draggingSeedPacket) {
                if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){gridCellPos.x, gridCellPos.y, gridCellSize.x, gridCellSize.y})) {
                    c = (Color){255, 255, 255, 100};
                }
            }
            DrawRectangleV(gridCellPos, gridCellSize, c);
        }
    }

    UpdateDrawPlants();
    UpdateDrawProjectiles();
    UpdateDrawParticles();
    UpdateDrawZombies();
    UpdateDrawSuns();

    // Draw top bar
    DrawRectangle(0, 0, screenWidth, SEEDPACKET_SIZE.y+20, DARKBROWN);

    // Draw seed tray
    int margin = 4;
    Vector2 trayStart = {seedPackets[0].origin.x-margin, seedPackets[0].origin.y-margin};
    Vector2 trayEnd = {trayStart.x+8*SEEDPACKET_SIZE.x+margin, seedPackets[0].origin.y+SEEDPACKET_SIZE.y};
    DrawRectangleV(trayStart, trayEnd, (Color){46, 40, 34, 255});

    UpdateDrawSeedPackets();

    // Draw sun count
    char sunCountText[32];
    sprintf(sunCountText, "Sun:\n\n%i", sunsCollectedCount);
    DrawText(sunCountText, 32, 10, 20, WHITE);


    // Draw timer
    frameCount++;
    char timerText[32];
    sprintf(timerText, "%i", frameCount/60);
    DrawText(timerText, screenWidth-64, 10, 20, WHITE);
}

void InitializeGame(void)
{
    CreateSeedPackets();

    InitSuns();

    InitZombies();

    // Init plant grid
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            gardenGrid[i][j].type = PT_NONE;
        }
    }

    SeekMusicStream(themeSong, 0);

}

void GameOver(void)
{
    currentScreen = GAME_SCREEN_GAMEOVER;
}

void DrawBackground(void)
{
    int tilesX = screenWidth/32;
    int tilesY = screenHeight/32;

    for (int x = 0; x < tilesX; x++) {
        for (int y = 0; y < tilesY; y++) {
            DrawTexture(lawnBackgroundSprite, x * 32, y * 32, WHITE);
        }
    }
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

void NextObject(int *next, int max)
{
    (*next)++;
    if (*next == max)
        *next = 0;
}

int GetUniqueRandomValue(int exludedValue, int min, int max)
{
    int random;
    do {
        random = GetRandomValue(min, max);
    } while (random == exludedValue);

    return random;
}

void DrawTextureCentered(Texture2D sprite, Vector2 pos, Vector2 origin, Color tint)
{
    Rectangle src = {0, 0, sprite.width, sprite.height};
    Rectangle dst = {EXPAND_V2(pos), sprite.width, sprite.height};
    DrawTexturePro(sprite, src, dst, origin, 0, tint);
}
