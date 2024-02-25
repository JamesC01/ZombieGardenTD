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
#include "ui.h"
#include <assert.h>

typedef enum {
    GAME_SCREEN_START,
    GAME_SCREEN_PLAYING,
    GAME_SCREEN_PAUSE_MENU,
    GAME_SCREEN_GAMEOVER,
    GAME_SCREEN_EXIT
} GameScreen;

GameScreen currentScreen = GAME_SCREEN_START;


FixedObjectArray projectiles;

const int virtualScreenWidth = 640;
const int virtualScreenHeight = 480;

Vector2 virtualMousePosition;

const int defaultFps = 60;
bool limitFrameRate = true;
bool paused = false;
bool playingMusic = true;
bool raining = false;
int frameCount = 0;

Plant gardenGrid[GRID_WIDTH][GRID_HEIGHT] = {0};
Vector2 gridDrawOffset = {40, 80};
Vector2 gridCellSize = {65, 78};

RenderTexture2D targetRT;

void UpdateDrawPauseMenu(void);
void UpdateDrawStart(void);
void UpdateDrawGame(void);
void UpdateDrawGameOver(void);
void UpdateDrawProjectiles(void);
void InitializeGame(void);
void DrawBackground(void);
Rectangle GetRenderRect(void);

void ReadWriteConfig(char *operation);

FixedObjectArray CreateFixedObjectArray(int objMaxCount, int typeSizeBytes);

int main(void)
{
    InitWindow(virtualScreenWidth, virtualScreenHeight, "Plants Vs Zombies Clone");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    InitAudioDevice();

    SetTargetFPS(defaultFps);
    SetExitKey(KEY_NULL);

    LoadAssets();

    SetMusicVolume(themeSong, 0.5f);
    PlayMusicStream(themeSong);

    SetMusicVolume(rainLoop, 0.25f);
    PlayMusicStream(rainLoop);

    projectiles = CreateFixedObjectArray(32, sizeof(Projectile));
    particles = CreateFixedObjectArray(2048, sizeof(Particle));
    zombies = CreateFixedObjectArray(64, sizeof(Zombie));
    suns = CreateFixedObjectArray(8, sizeof(Sun));

    InitializeGame();

    targetRT = LoadRenderTexture(640, 480);

    ReadWriteConfig("r");


    bool shouldClose = false;
    while (!WindowShouldClose() && !shouldClose) {

        Rectangle scaledScreen = GetRenderRect();

        // Calculate virtual mouse position
        Vector2 mPos = GetMousePosition();
        mPos.x -= scaledScreen.x;
        mPos.y -= scaledScreen.y;
        virtualMousePosition.x = mPos.x / (scaledScreen.width/virtualScreenWidth);
        virtualMousePosition.y = mPos.y / (scaledScreen.height/virtualScreenHeight);

        // Toggle frame limiter
        if (IsKeyPressed(KEY_S)) {
            limitFrameRate = !limitFrameRate;
            if (limitFrameRate) {
                SetTargetFPS(defaultFps);
            } else {
                SetTargetFPS(0);
            }
        }

        if (IsKeyPressed(KEY_R)) {
            raining = !raining;
        }



        if (raining) {
            UpdateMusicStream(rainLoop);

            // Spawn rain particles
            Vector2 randPos = {
                GetRandomValue(0, virtualScreenWidth),
                GetRandomValue(-virtualScreenHeight/2, 0)
            };
            Vector2 randSize = Vector2Scale((Vector2){3, 8}, GetRandomFloatValue(0.5f, 1));
            Particle* p = CreateParticle(P_RAIN, randPos, randSize, GetRandomValue(10, 60*5), (Color){50, 200, 220, 80});
            p->velocity = (Vector2){
                0,
                    GetRandomFloatValue(8, 16)
            };
        }


        BeginTextureMode(targetRT);

        ClearBackground(WHITE);
        switch (currentScreen) {
            case GAME_SCREEN_START:
                UpdateDrawStart();
                break;
            case GAME_SCREEN_PLAYING:
                UpdateDrawGame();
                break;
            case GAME_SCREEN_PAUSE_MENU:
                UpdateDrawPauseMenu();
                break;
            case GAME_SCREEN_GAMEOVER:
                UpdateDrawGameOver();
                break;
            case GAME_SCREEN_EXIT:
                shouldClose = true;
                break;
        }

        if (raining) {
            DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, (Color){0, 20, 50, 75});
        }

        // Draw FPS
        char fpsText[32];
        sprintf(fpsText, "%ifps", GetFPS());
        DrawTextWithShadow(smallFont, fpsText, 16, virtualScreenHeight-35, 35, 2, WHITE);

        EndTextureMode();

        // Scale render texture to screen
        BeginDrawing();

        // Strange bug, can't clear the screen, it messes with transparent stuff.

        Rectangle src = {0, 0, virtualScreenWidth, -virtualScreenHeight};
        DrawTexturePro(targetRT.texture, src, GetRenderRect(), Vector2Zero(), 0, WHITE);


        EndDrawing();


    }

    ReadWriteConfig("w");

    free(projectiles.array);
    free(particles.array);
    free(zombies.array);
    free(suns.array);

    UnloadRenderTexture(targetRT);

    UnloadAssets();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void UpdateDrawStart(void)
{
    DrawBackground();

    char *titleText1 = "PvZ";
    char *titleText2 = "Clone";
    DrawTextWithShadow(bigFont, titleText1, GetCenteredTextX(bigFont, 75, titleText1), 32, 75, 4, WHITE);
    DrawTextWithShadow(bigFont, titleText2, GetCenteredTextX(bigFont, 75, titleText2), 32+75, 75, 4, WHITE);

    int x = 16;
    int y = virtualScreenHeight/2-30;
    int height = 60;

    TextOptions options = {
        smallFont,
        40,
        2,
        WHITE
    };

    ButtonOptions bOptions = defaultButtonOptions;
    bOptions.centered = true;
    bOptions.colour = GREEN;
    bOptions.shadowOffset = 4;

    const int btnShadow = 4;
    if (TextButton(bOptions, options, "Start Game!", x, y)) {
        currentScreen = GAME_SCREEN_PLAYING;
        InitializeGame();
    }

    y += height;

    bOptions.colour = RED;

    if (TextButton(bOptions, options, "Quit Game", x, y)) {
        currentScreen = GAME_SCREEN_EXIT;
    }


    char *creditText = "Game by James Czekaj";
    DrawTextWithShadow(smallFont, creditText, GetCenteredTextX(smallFont, 25, creditText), virtualScreenHeight-32, 25, 2, WHITE);
}


void UpdateDrawPauseMenu(void)
{
    DrawBackground();

    char *pausedText = "Paused";
    DrawTextWithShadow(bigFont, pausedText, GetCenteredTextX(bigFont, 50, pausedText), 64, 50, 4, WHITE);

    int x;
    int y = virtualScreenHeight/2-80;
    int height = 50;

    TextOptions options = {
        smallFont,
        30,
        2,
        WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.shadowOffset = 4;
    bOpt.centered = true;
    bOpt.colour = GREEN;
    bOpt.minWidth = 200;

    const int btnShadow = 4;
    if (TextButton(bOpt, options, "Resume", x, y)) {
        currentScreen = GAME_SCREEN_PLAYING;
    }

    y += height;

    bOpt.colour = (Color){0, 150, 200, 255};

    char rainText[32];
    sprintf(rainText, "Rain (%s)", (raining) ? "On" : "Off");
    x = GetCenteredTextX(options.font, options.size, rainText);
    if (TextButton(bOpt, options, rainText, x, y)) {
        raining = !raining;
    }

    y += height;

    bOpt.colour = BROWN;

    char musicText[32];
    sprintf(musicText, "Music (%s)", (playingMusic) ? "On" : "Off");
    if (TextButton(bOpt, options, musicText, x, y)) {
        playingMusic = !playingMusic;
    }

    y += height;

    bOpt.colour = LIGHTGRAY;

    char fullScreenText[32];
    sprintf(fullScreenText, "Fullscreen (%s)", (IsWindowFullscreen()) ? "Yes" : "No");
    if (TextButton(bOpt, options, fullScreenText, x, y)) {
        ToggleFullscreen();
    }

    y += height;

    bOpt.colour = RED;

    if (TextButton(bOpt, options, "Give Up", x, y)) {
        currentScreen = GAME_SCREEN_START;
    }
}

void UpdateDrawGameOver(void)
{
    DrawBackground();

    DrawTextWithShadow(bigFont, "YOU DIED!", GetCenteredTextX(bigFont, 50, "YOU DIED!"), 32, 50, 4, WHITE);
    DrawTextWithShadow(bigFont, "GAME OVER", GetCenteredTextX(bigFont, 50, "GAME OVER"), 80, 50, 4, WHITE);

    char killCountText[32];
    sprintf(killCountText, "You killed %i zombies!", zombiesKilledCount);
    DrawTextWithShadow(smallFont, killCountText, GetCenteredTextX(smallFont, 40, killCountText), virtualScreenHeight/2-80, 40, 2, WHITE);

    TextOptions options = {
        smallFont,
        30,
        2,
        WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.colour = LIGHTGRAY;
    bOpt.centered = true;

    if (TextButton(bOpt, options, "Return to Start", 16, virtualScreenHeight/2)) {
        currentScreen = GAME_SCREEN_START;
    }
}

void UpdateDrawGame(void)
{
    if (IsKeyPressed(KEY_M)) {
        playingMusic = !playingMusic;
    }


    if (playingMusic) {
        UpdateMusicStream(themeSong);
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !draggingSeedPacket) {
        CreateParticleConfetti(GetMousePosVirtual(), (Vector2){4, 4}, 4);
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        CreateParticleExplosion(GetMousePosVirtual(), (Vector2){4, 4}, 4, 10, 16, DARKBROWN);
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        currentScreen = GAME_SCREEN_PAUSE_MENU;
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
                if (CheckCollisionPointRec(GetMousePosVirtual(), (Rectangle){gridCellPos.x, gridCellPos.y, gridCellSize.x, gridCellSize.y})) {
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
    int tilesX = virtualScreenWidth/woodBackgroundSprite.width;
    Rectangle src = {0,0,woodBackgroundSprite.width,woodBackgroundSprite.height};
    for (int x = 0; x < tilesX; x++) {
        Rectangle dst = {x*32, 0, 32, SEEDPACKET_SIZE.y+22};
        DrawTexturePro(woodBackgroundSprite, src, dst, Vector2Zero(), 0, WHITE);
    }

    // Draw seed tray
    int margin = 4;
    Vector2 trayStart = {seedPackets[0].origin.x-margin, seedPackets[0].origin.y-margin};
    Vector2 trayEnd = {trayStart.x+8*SEEDPACKET_SIZE.x+margin, seedPackets[0].origin.y+SEEDPACKET_SIZE.y};
    Rectangle tray = {
        trayStart.x, trayStart.y,
        trayEnd.x, trayEnd.y
    };
    Rectangle border = tray;
    border.x -= 1;
    border.y -= 1;
    border.width += 2;
    border.height += 2;
    DrawRectangleRec(border, (Color){255, 220, 200, 100});
    DrawRectangleRec(tray, (Color){46, 40, 34, 255});


    UpdateDrawSeedPackets();

    const int fSize = 30;
    // Draw sun count
    char sunCountText[32];
    sprintf(sunCountText, "%i", sunsCollectedCount);
    int sY = 10;
    DrawTextWithShadow(smallFont, "Sun:", 32, sY, fSize, 1, WHITE);
    DrawTextWithShadow(smallFont, sunCountText, 32, sY+fSize*0.75f, fSize, 1, WHITE);


    // Draw timer
    frameCount++;
    char timerText[32];
    sprintf(timerText, "%i", frameCount/60);
    DrawTextWithShadow(smallFont, timerText, virtualScreenWidth-100, 10, 40, 1, WHITE);

    TextOptions options = {
        smallFont,
        20,
        2,
        WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.centered = false;

    if (TextButton(bOpt, options, "||", virtualScreenWidth-32, virtualScreenHeight-32)) {
        currentScreen = GAME_SCREEN_PAUSE_MENU;
    }
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
    int tilesX = virtualScreenWidth/32;
    int tilesY = virtualScreenHeight/32;

    for (int x = 0; x < tilesX; x++) {
        for (int y = 0; y < tilesY; y++) {
            DrawTexture(lawnBackgroundSprite, x * 32, y * 32, WHITE);
        }
    }

}

void UpdateDrawProjectiles(void)
{
    // Collision behaviour is done inside zombie update
    Projectile* projArr = (Projectile*)projectiles.array;
    for (int i = 0; i < projectiles.fixedSize; i++) {
        Projectile* proj = &projArr[i];

        if (proj->active) {
            proj->pos.x += 5;
            DrawTextureCentered(smallShadowSprite, Vector2Add(proj->pos, (Vector2){0, 42}), Vector2Zero(), WHITE);
            DrawTextureV(peaSprite, proj->pos, WHITE);

            if (proj->pos.x > 640) {
                proj->active = false;
            }
        }
    }
}

void NextObject(FixedObjectArray* array)
{
    array->next++;
    if (array->next == array->fixedSize)
        array->next = 0;
}

int GetUniqueRandomValue(int exludedValue, int min, int max)
{
    int random;
    do {
        random = GetRandomValue(min, max);
    } while (random == exludedValue);

    return random;
}

float GetRandomFloatValue(float min, float max)
{
    assert(min < max && "min less than max");

    int wholeDifference = fabs(min-max);
    float fractionalDifference = fabs(min-max) - wholeDifference; 

    float wholeRandom = (rand()/(float)RAND_MAX) * wholeDifference;
    float fracRandom = (rand()/(float)RAND_MAX) * fractionalDifference;


    return min + wholeRandom + fracRandom;
}

// TODO: This doesn't seem to do any centering?
void DrawTextureCentered(Texture2D sprite, Vector2 pos, Vector2 origin, Color tint)
{
    Rectangle src = {0, 0, sprite.width, sprite.height};
    Rectangle dst = {EXPAND_V2(pos), sprite.width, sprite.height};
    DrawTexturePro(sprite, src, dst, origin, 0, tint);
}

bool TickCooldown(int *timer, int cooldownTime)
{
    (*timer)--;
    if (*timer <= 0) {
        *timer = cooldownTime;
        return true;
    }

    return false;
}


FixedObjectArray CreateFixedObjectArray(int objMaxCount, int typeSizeBytes)
{
    FixedObjectArray objArray;
    objArray.array = malloc(typeSizeBytes * objMaxCount);
    memset(objArray.array, 0, typeSizeBytes * objMaxCount);
    objArray.fixedSize = objMaxCount;
    objArray.next = 0;

    return objArray;
}

Vector2 GetMousePosVirtual(void)
{
    return virtualMousePosition;
}

Rectangle GetRenderRect(void)
{
    int windowWidth = GetScreenWidth();
    int windowHeight = GetScreenHeight();
    bool landscape = windowWidth > windowHeight;

    int dstWidth = windowWidth;
    int dstHeight = windowHeight;
    int offsetX = 0;
    int offsetY = 0;

    float scale;
    if (landscape) {
        scale = (float)windowHeight/virtualScreenHeight;
        dstWidth = virtualScreenWidth*scale;
        offsetX = (windowWidth-dstWidth)/2;
    } else {
        scale = (float)windowWidth/virtualScreenWidth;
        dstHeight = scale*virtualScreenHeight;
        offsetY = (windowHeight-dstHeight)/2;
    }

    return (Rectangle){offsetX, offsetY, dstWidth, dstHeight};
}

// Read/Write config options (music on/off, rain on/off etc.)
// operation should be r for read, w for write
void ReadWriteConfig(char *operation)
{
    FILE* configFile = fopen(".zombieconfig", operation);

    if (strcmp(operation, "r") == 0) {
        int fullscreen;
        int _playingMusic;
        int _raining;
        fscanf(configFile, "%i %i %i", &_raining, &_playingMusic, &fullscreen);

        if (fullscreen) {
            ToggleFullscreen();
        }

        raining = (bool)_raining;
        playingMusic = (bool)_playingMusic;
    } else if (strcmp(operation, "w") == 0) {
        bool fullscreen = IsWindowFullscreen();
        fprintf(configFile, "%i %i %i", raining, playingMusic, fullscreen);
    }

    fclose(configFile);
}
