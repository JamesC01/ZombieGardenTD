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
#include <assert.h>

typedef enum {
    GAME_SCREEN_START,
    GAME_SCREEN_PLAYING,
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
int frameCount = 0;

Plant gardenGrid[GRID_WIDTH][GRID_HEIGHT] = {0};
Vector2 gridDrawOffset = {40, 80};
Vector2 gridCellSize = {65, 78};

RenderTexture2D targetRT;

void UpdateDrawStart(void);
void UpdateDrawGame(void);
void UpdateDrawGameOver(void);
void UpdateDrawProjectiles(void);
void InitializeGame(void);
void DrawBackground(void);
Rectangle GetRenderRect(void);

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

    projectiles = CreateFixedObjectArray(16, sizeof(Projectile));
    particles = CreateFixedObjectArray(2048, sizeof(Particle));
    zombies = CreateFixedObjectArray(32, sizeof(Zombie));
    suns = CreateFixedObjectArray(8, sizeof(Sun));

    InitializeGame();

    targetRT = LoadRenderTexture(640, 480);
    SetTextureFilter(targetRT.texture, TEXTURE_FILTER_BILINEAR);

    bool playingMusic = true; // TODO: should be true by default
    bool raining = false;
    

    bool shouldClose = false;
    while (!WindowShouldClose() && !shouldClose) {

        Rectangle scaledScreen = GetRenderRect();

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

        TextOptions options = {
            smallFont,
            20,
            2,
            WHITE
        };
        
        if (TextButton(options, "Growl!", 100, 100, GREEN, 8)) {
            PlaySound(zombieGrowlSounds[0]);
        }

        EndTextureMode();

        // Scale render texture to screen
        BeginDrawing();

        // Strange bug, can't clear the screen, it messes with transparent stuff.

        Rectangle src = {0, 0, virtualScreenWidth, -virtualScreenHeight};
        DrawTexturePro(targetRT.texture, src, GetRenderRect(), Vector2Zero(), 0, WHITE);

        EndDrawing();


    }

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

// TODO: Maybe make it so you can't click outside the button, then move into the button and let go to click
bool TextButton(TextOptions textOptions, char *text, int x, int y, Color buttonColour, int buttonShadowOffset)
{
    const int padding = 4;
    Vector2 textSize = MeasureTextEx(textOptions.font, text, textOptions.size, 0);
    Rectangle button = {
        x, y,
        textSize.x+padding, textSize.y+padding
    };

    Rectangle shadow = button;
    Rectangle outline = button;
    const int outlineThickness = 2;
    outline.x -= outlineThickness;
    outline.y -= outlineThickness;
    outline.width += outlineThickness*2;
    outline.height += outlineThickness*2;

    bool pressed = false;

    if (CheckCollisionPointRec(virtualMousePosition, button)) {
        // TODO: clean this up
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !draggingSeedPacket) {
            buttonColour.r *= 0.5f;
            buttonColour.g *= 0.5f;
            buttonColour.b *= 0.5f;
        } else {
            buttonColour.r *= 0.75f;
            buttonColour.g *= 0.75f;
            buttonColour.b *= 0.75f;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !draggingSeedPacket) {
            pressed = true;
        }

        shadow.x += (float)buttonShadowOffset/2;
        shadow.y += (float)buttonShadowOffset/2;
    } else {
        shadow.x += buttonShadowOffset;
        shadow.y += buttonShadowOffset;
    }

    DrawRectangleRec(shadow, (Color){0, 0, 0, 100});
    DrawRectangleRec(outline, BLACK);
    DrawRectangleRec(button, buttonColour);

    DrawTextWithShadow(textOptions.font, text, x+padding/2, y+padding/2, textOptions.size, textOptions.shadowOffset, textOptions.colour);

    return pressed;
}

void UpdateDrawStart(void)
{
    DrawBackground();

    DrawTextWithShadow(bigFont, "Raylib\n\n\nPlants Vs Zombies\n\n\nClone", 16, 32, 50, 4, WHITE);

    int x = 16;
    int y = virtualScreenHeight/2;
    int height = 35;

    DrawTextWithShadow(smallFont, "Press Enter to Start Game", x, y, 35, 2, GREEN);
    y += height;

    DrawTextWithShadow(smallFont, "Press Q to Quit Game", x, y, 35, 2, RED);

    y += height;
    y += height;

    DrawTextWithShadow(smallFont, "Game by James Czekaj", x, y, 25, 2, WHITE);


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

    DrawTextWithShadow(bigFont, "YOU DIED!\n\n\n\nGAME OVER :(", 16, 16, 50, 4, WHITE);

    char killCountText[32];
    sprintf(killCountText, "You killed %i zombies!", zombiesKilledCount);
    DrawTextWithShadow(smallFont, killCountText, 16, virtualScreenHeight/2-40, 40, 2, WHITE);

    DrawTextWithShadow(smallFont, "Press Enter to return to Start", 16, virtualScreenHeight/2, 40, 2, WHITE);

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
    DrawRectangle(0, 0, virtualScreenWidth, SEEDPACKET_SIZE.y+20, DARKBROWN);

    // Draw seed tray
    int margin = 4;
    Vector2 trayStart = {seedPackets[0].origin.x-margin, seedPackets[0].origin.y-margin};
    Vector2 trayEnd = {trayStart.x+8*SEEDPACKET_SIZE.x+margin, seedPackets[0].origin.y+SEEDPACKET_SIZE.y};
    DrawRectangleV(trayStart, trayEnd, (Color){46, 40, 34, 255});

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

void DrawTextWithShadow(Font font, const char *text, int x, int y, float fontSize, float shadowOffset, Color tint)
{
    const Color shadowColour = {0, 0, 0, 150};
    DrawTextEx(font, text, (Vector2){x+shadowOffset, y+shadowOffset}, fontSize, 0, shadowColour);
    DrawTextEx(font, text, (Vector2){x, y}, fontSize, 0, tint);
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
