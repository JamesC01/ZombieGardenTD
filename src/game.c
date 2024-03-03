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
    GAME_SCREEN_CONFIG_MENU,
    GAME_SCREEN_EXIT
} GameScreen;

GameScreen previousScreen;
GameScreen currentScreen = GAME_SCREEN_START;

typedef struct {
    bool playingMusic;
    bool raining;
    bool fullscreen;
    int width, height;
    bool maximized;
} GameConfig;

FixedObjectArray projectiles;
FixedObjectArray drawDatas;

const int virtualScreenWidth = 640;
const int virtualScreenHeight = 480;

Vector2 virtualMousePosition;

const int defaultFps = 60;
bool limitFrameRate = true;
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
void UpdateDrawConfigMenu(GameConfig *config, GameScreen previousScreen);
void InitializeGame(void);
void DrawBackground(void);
Rectangle GetRenderRect(void);
void ChangeGameScreen(GameScreen newScreen);
int CompareDrawDatas(const void *a, const void *b);

void ReadWriteConfig(GameConfig *config, char *operation);

FixedObjectArray CreateFixedObjectArray(int objMaxCount, int typeSizeBytes);


GameConfig gameConfig = {
    .playingMusic = true,
    .raining = false,
    .fullscreen = false,
    .width = 640,
    .height = 480
};

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
    zombieHeads = CreateFixedObjectArray(64, sizeof(ZombieHead));
    suns = CreateFixedObjectArray(8, sizeof(Sun));
    drawDatas = CreateFixedObjectArray(128, sizeof(DrawData));

    targetRT = LoadRenderTexture(640, 480);

    ReadWriteConfig(&gameConfig, "rb");

    if (gameConfig.maximized) {
        SetWindowState(FLAG_WINDOW_MAXIMIZED);
    } else {
        SetWindowSize(gameConfig.width, gameConfig.height);
    }

    if (gameConfig.fullscreen && !IsWindowFullscreen()) {
        ToggleFullscreen();
    }

    bool shouldClose = false;
    while (!WindowShouldClose() && !shouldClose) {

        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

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
            int fps = (limitFrameRate) ? defaultFps : 0;
            SetTargetFPS(fps);
        }

        if (gameConfig.raining) {
            UpdateMusicStream(rainLoop);

            // Spawn rain particles
            for (int i = 0; i < 10; i++) {
                Vector2 randPos = {
                    GetRandomValue(0, virtualScreenWidth),
                    GetRandomValue(-virtualScreenHeight/2, 0)
                };
                Vector2 randSize = Vector2Scale((Vector2){2, 8}, GetRandomFloatValue(0.5f, 1));
                Particle* p = CreateParticle(P_RAIN, randPos, randSize, GetRandomValue(30, 60*5), (Color){220, 220, 255, 150});
                // TODO: Consider making rain fall down at a slight angle
                p->velocity.x = 0;
                p->velocity.y = GetRandomFloatValue(5, 12);
            }
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
            case GAME_SCREEN_CONFIG_MENU:
                UpdateDrawConfigMenu(&gameConfig, previousScreen);
                break;
        }


        if (gameConfig.raining) {
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

    gameConfig.width = GetScreenWidth();
    gameConfig.height = GetScreenHeight();
    gameConfig.maximized = IsWindowState(FLAG_WINDOW_MAXIMIZED);
    ReadWriteConfig(&gameConfig, "wb");

    free(projectiles.array);
    free(particles.array);
    free(zombies.array);
    free(suns.array);
    free(zombieHeads.array);
    free(drawDatas.array);

    UnloadRenderTexture(targetRT);

    UnloadAssets();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void UpdateDrawStart(void)
{
    DrawBackground();

    char *titleText1 = "Zombies";
    char *titleText2 = "In Your";
    char *titleText3 = "Garden";
    int tfSize = 75;
    DrawTextWithShadow(bigFont, titleText1, GetCenteredTextX(bigFont, tfSize, titleText1), 32, tfSize, 4, WHITE);
    DrawTextWithShadow(bigFont, titleText2, GetCenteredTextX(bigFont, tfSize/2, titleText1), 32+tfSize, (float)tfSize/2, 4, WHITE);
    DrawTextWithShadow(bigFont, titleText3, GetCenteredTextX(bigFont, tfSize, titleText3), 32+tfSize*1.5f, tfSize, 4, WHITE);


    TextOptions tOpt = {
        .font = smallFont,
        .size = 40,
        .shadowOffset = 2,
        .colour = WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.centered = true;
    bOpt.shadowOffset = 4;

    int y = virtualScreenHeight/2;
    int gap = 6;
    int height = GetButtonHeight(bOpt, tOpt) + gap;

    bOpt.colour = GREEN;
    if (TextButton(bOpt, tOpt, "Start Game!", 0, y)) {
        ChangeGameScreen(GAME_SCREEN_PLAYING);
        InitializeGame();
    }

    y += height;
    bOpt.colour = LIGHTGRAY;
    if (TextButton(bOpt, tOpt, "Game Config", 0, y)) {
        ChangeGameScreen(GAME_SCREEN_CONFIG_MENU);
    }

    y += height;
    bOpt.colour = RED;
    if (TextButton(bOpt, tOpt, "Quit Game", 0, y)) {
        ChangeGameScreen(GAME_SCREEN_EXIT);
    }


    char *creditText = "(c) James Czekaj 2024";
    int cfSize = 25;
    DrawTextWithShadow(smallFont, creditText, GetCenteredTextX(smallFont, cfSize, creditText), virtualScreenHeight-32, cfSize, 2, WHITE);

    char *versionText = VERSION_STRING;
    int vfSize = 25;
    DrawTextWithShadow(smallFont, versionText, 8, 5, cfSize, 2, WHITE);
}

void UpdateDrawConfigMenu(GameConfig *config, GameScreen previousScreen)
{
    DrawBackground();

    char *pausedText = "Game Config";
    int tfSize = 50;
    DrawTextWithShadow(bigFont, pausedText, GetCenteredTextX(bigFont, tfSize, pausedText), 64, tfSize, 4, WHITE);


    TextOptions tOpt = {
        .font = smallFont,
        .size = 40,
        .shadowOffset = 2,
        .colour = WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.shadowOffset = 4;
    bOpt.centered = true;
    bOpt.minWidth = 250;

    int y = virtualScreenHeight/2-80;
    int gap = 6;
    int height = GetButtonHeight(bOpt, tOpt) + gap;

    bOpt.colour = LIGHTGRAY;
    const int btnShadow = 4;
    if (TextButton(bOpt, tOpt, "<-- Back", 0, y)) {
        ChangeGameScreen(previousScreen);
    }

    y += height;

    bOpt.colour = (Color){0, 150, 200, 255};
    char rainText[32];
    sprintf(rainText, "Rain (%s)", (gameConfig.raining) ? "On" : "Off");
    if (TextButton(bOpt, tOpt, rainText, 0, y)) {
        gameConfig.raining = !gameConfig.raining;
    }

    y += height;

    bOpt.colour = BROWN;
    char musicText[32];
    sprintf(musicText, "Music (%s)", (gameConfig.playingMusic) ? "On" : "Off");
    if (TextButton(bOpt, tOpt, musicText, 0, y)) {
        gameConfig.playingMusic = !gameConfig.playingMusic;
    }

    y += height;

    bOpt.colour = DARKPURPLE;
    char fullScreenText[32];
    sprintf(fullScreenText, "Fullscreen (%s)", (IsWindowFullscreen()) ? "Yes" : "No");
    if (TextButton(bOpt, tOpt, fullScreenText, 0, y)) {
        ToggleFullscreen();
        gameConfig.fullscreen = IsWindowFullscreen();
    }
}


void UpdateDrawPauseMenu(void)
{
    DrawBackground();

    char *pausedText = "Paused";
    int tfSize = 50;
    DrawTextWithShadow(bigFont, pausedText, GetCenteredTextX(bigFont, tfSize, pausedText), 64, tfSize, 4, WHITE);


    TextOptions tOpt = {
        .font = smallFont,
        .size = 40,
        .shadowOffset = 2,
        .colour = WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.shadowOffset = 4;
    bOpt.centered = true;
    bOpt.minWidth = 250;

    int y = virtualScreenHeight/2-80;
    int gap = 6;
    int height = GetButtonHeight(bOpt, tOpt) + gap;

    bOpt.colour = GREEN;
    if (TextButton(bOpt, tOpt, "Resume", 0, y)) {
        ChangeGameScreen(GAME_SCREEN_PLAYING);
    }

    y += height;
    bOpt.colour = YELLOW;
    if (TextButton(bOpt, tOpt, "Give Sun", 0, y)) {
        sunsCollectedCount += 100*SUN_VALUE;
    }

    y += height;
    bOpt.colour = GREEN;
    // Temporarily here
    if (TextButton(bOpt, tOpt, "Toggle Right-click Zombie spawning", 0, y)) {
        debugZombieSpawning = !debugZombieSpawning;
    }

    y += height;
    bOpt.colour = LIGHTGRAY;
    if (TextButton(bOpt, tOpt, "Game Config", 0, y)) {
        ChangeGameScreen(GAME_SCREEN_CONFIG_MENU);
    }

    y += height;
    bOpt.colour = RED;
    if (TextButton(bOpt, tOpt, "Give Up", 0, y)) {
        ChangeGameScreen(GAME_SCREEN_START);
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
        .font = smallFont,
        .size = 40,
        .shadowOffset = 2,
        .colour = WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.colour = LIGHTGRAY;
    bOpt.centered = true;

    if (TextButton(bOpt, options, "Return to Start", 0, virtualScreenHeight/2)) {
        ChangeGameScreen(GAME_SCREEN_START);
    }
}

void UpdateDrawGame(void)
{
    if (gameConfig.playingMusic) {
        UpdateMusicStream(themeSong);
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) && !draggingSeedPacket) {
        CreateParticleConfetti(GetMousePosVirtual(), (Vector2){4, 4}, 6);
        if (!IsSoundPlaying(popSound)) {
            SetSoundPitch(popSound, GetRandomFloatValue(1.8f, 2));
            PlaySound(popSound);
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        CreateParticleExplosion(GetMousePosVirtual(), (Vector2){4, 4}, 4, 10, 16, DARKBROWN);
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        ChangeGameScreen(GAME_SCREEN_PAUSE_MENU);
    }

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

    UpdateDrawProjectiles();

    UpdateDrawZombieHeads();
    UpdateDrawZombies();
    UpdateDrawPlants();

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
    // TODO: Consider making a DrawRectangleWithBorder function that could be used here, and by TextButton
    Rectangle border = tray;
    border.x -= 1;
    border.y -= 1;
    border.width += 2;
    border.height += 2;
    DrawRectangleRec(border, (Color){255, 220, 200, 100});
    DrawRectangleRec(tray, (Color){46, 40, 34, 255});

    // Depth sorted drawing 
    qsort(drawDatas.array, drawDatas.fixedSize, sizeof(DrawData), &CompareDrawDatas);
    for (int i = 0; i < drawDatas.fixedSize; i++) {
        DrawData *dd = &((DrawData*)drawDatas.array)[i];
        if (dd->shouldDraw) {
            DrawTextureFull(dd->sprite, dd->pos, dd->origin, dd->tint, dd->scale, dd->rotation);
            dd->shouldDraw = false;
        }
    }

    UpdateDrawParticles();
    UpdateDrawSuns();



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
        .font = smallFont,
        .size = 20,
        .shadowOffset = 2,
        .colour = WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.paddingX = 16;
    bOpt.paddingY = 8;
    bOpt.centered = false;

    if (TextButton(bOpt, options, "||", virtualScreenWidth-32, virtualScreenHeight-32)) {
        ChangeGameScreen(GAME_SCREEN_PAUSE_MENU);
    }

}

void InitializeGame(void)
{
    frameCount = 0;
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
    ChangeGameScreen(GAME_SCREEN_GAMEOVER);
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
            DrawTextureFull(smallShadowSprite, Vector2Add(proj->pos, (Vector2){0, 42}), Vector2Zero(), WHITE, 1, 0);
            DrawTextureV(seedSprite, proj->pos, WHITE);

            if (proj->pos.x > 640) {
                proj->active = false;
            }
        }
    }
}

void IncrementArrayIndex(FixedObjectArray *array)
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
void DrawTextureFull(Texture2D sprite, Vector2 pos, Vector2 origin, Color tint, float scale, float rotation)
{
    Rectangle src = {0, 0, sprite.width, sprite.height};
    Rectangle dst = {EXPAND_V2(pos), sprite.width*scale, sprite.height*scale};
    DrawTexturePro(sprite, src, dst, Vector2Scale(origin, scale), rotation, tint);
}

void PushDrawData(Texture2D sprite, int depth, Vector2 pos, Vector2 origin, Color tint, float scale, float rotation)
{
    DrawData *dd = GetNextObject(drawDatas, DrawData);
    IncrementArrayIndex(&drawDatas);
    dd->sprite = sprite;
    dd->pos = pos;
    dd->origin = origin;
    dd->tint = tint;
    dd->scale = scale;
    dd->rotation = rotation;
    dd->depth = depth;

    dd->shouldDraw = true;
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

    float scale;
    bool landscape = windowWidth > windowHeight;
    if (landscape)
        scale = (float)windowHeight/virtualScreenHeight;
    else
        scale = (float)windowWidth/virtualScreenWidth;

    Rectangle renderRect;
    renderRect.width = virtualScreenWidth*scale;
    renderRect.height = virtualScreenHeight*scale;
    renderRect.x = (windowWidth-renderRect.width)/2;
    renderRect.y = (windowHeight-renderRect.height)/2;

    return renderRect;
}

// Read/Write config options (music on/off, rain on/off etc.)
// operation should be r for read, w for write
void ReadWriteConfig(GameConfig *config, char *operation)
{
    FILE* configFile = fopen(".zombieconfig", operation);

    if (strcmp(operation, "rb") == 0 && configFile) {
        fread(config, sizeof(GameConfig), 1, configFile);
    } else if (strcmp(operation, "wb") == 0 && configFile) {
        fwrite(config, sizeof(GameConfig), 1, configFile);
    }

    if (configFile) {
        fclose(configFile);
    }
} 

Vector2 GetTextureCenterPoint(Texture2D sprite)
{
    return (Vector2){sprite.width/2.0f, sprite.height/2.0f};
}

int GetButtonHeight(ButtonOptions bOpt, TextOptions tOpt)
{
    return tOpt.size + bOpt.outlineThickness*2 + bOpt.paddingY;
}

void ChangeGameScreen(GameScreen newScreen)
{
    previousScreen = currentScreen;
    currentScreen = newScreen;
}

int CompareDrawDatas(const void *a, const void *b)
{
    DrawData *d1 = (DrawData*)a;
    DrawData *d2 = (DrawData*)b;

    if (d1->depth < d2->depth)
        return -1;
    if (d1->depth > d2->depth)
        return 1;

    return 0;
}
