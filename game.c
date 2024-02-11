#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>

#define EXPAND_V2(v2) v2.x, v2.y

typedef enum {
    PT_NONE,
    PT_PSHOOTER,
    PT_SUNFLOWER,
    PT_WALLNUT,
    PT_CHERRYBOMB,
    PT_COUNT
} PlantType;

typedef struct {
    PlantType type;
    // packet sprite
    Vector2 origin;
    int cost;
    bool dragging;
} SeedPacket;
const Vector2 SEEDPACKET_SIZE = {40, 50};

typedef struct {
    PlantType type;
    int cooldown;
    float health;
} Plant;

typedef struct {
    Vector2 pos;
    bool active;
} Projectile;

typedef struct {
    Vector2 pos;
    bool active;
    bool isBigSun;
} Sun;


void UpdateDrawPShooter(Plant* p, Vector2 screenPos);
void UpdateDrawSeedPackets();

// Assets
Texture2D seedPacketSprite;
Texture2D lawnBackgroundSprite;
Texture2D pShooterSprite;
Texture2D peaSprite;
Texture2D sunSprite;

// Projectile globals
#define MAX_PROJ 16
Projectile projectiles[MAX_PROJ] = {0};
int nextProjectile = 0;

// Plant Grid globals
Plant gardenGrid[9][5] = {0};
Vector2 gridDrawOffset = {40, 80};
int gridCellGap = 0;
Vector2 gridCellSize = {55, 70};

// Seed Packet globals
SeedPacket seedPackets[2];

// Sun globals
#define MAX_SUNS 8
Sun suns[MAX_SUNS] = {0};
int nextSun = 0;

int sunCooldown = 60;
int sunsCollectedCount = 0;

int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "Plants Vs Zombies Clone");

    SetTargetFPS(60);

    seedPacketSprite = LoadTexture("sprites/seedpacket.png");
    lawnBackgroundSprite = LoadTexture("sprites/lawn.png");
    pShooterSprite = LoadTexture("sprites/pshooter.png");
    peaSprite = LoadTexture("sprites/pea.png");
    sunSprite = LoadTexture("sprites/sun.png");

    seedPackets[0] = (SeedPacket){ PT_NONE, (Vector2){100, 10}};
    seedPackets[1] = (SeedPacket){ PT_PSHOOTER, (Vector2){100 + SEEDPACKET_SIZE.x + 8, 10}};

    while (!WindowShouldClose()) {
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
        for (int x = 0; x < 9; x++) {
            for (int y = 0; y < 5; y++) {

                // Checkerboard tint
                Color c = {230, 255, 230, 50};
                bool xEven = (x % 2) == 0;
                bool yEven = (y % 2) == 0;
                if ((xEven && yEven) || (!xEven && !yEven)) {
                    c = (Color){230, 255, 230, 20};
                }

                Vector2 gridCellPos = Vector2Add(gridDrawOffset, (Vector2){x*gridCellSize.x+x*gridCellGap, y*gridCellSize.y+y*gridCellGap});

                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){gridCellPos.x, gridCellPos.y, gridCellSize.x, gridCellSize.y})) {
                        c = (Color){255, 255, 255, 100};
                    }
                }
                DrawRectangleV(gridCellPos, gridCellSize, c);
            }
        }

        // Update and Draw plants
        for (int x = 0; x < 9; x++) {
            for (int y = 0; y < 5; y++) {
                Vector2 screenPos = Vector2Add(gridDrawOffset, (Vector2){x*gridCellSize.x+x*gridCellGap, y*gridCellSize.y+y*gridCellGap});
                Plant* p = &gardenGrid[x][y];
                switch (p->type) {
                    case PT_PSHOOTER:
                        UpdateDrawPShooter(p, screenPos);
                        break;
                    case PT_NONE:
                    default:
                        continue;
                }
            }
        }

        // Update and draw Projectiles
        for (int i = 0; i < MAX_PROJ; i++) {
            if (projectiles[i].active) {
                projectiles[i].pos.x += 5;
                DrawTextureV(peaSprite, projectiles[i].pos, WHITE);
            }
        }


        // Update and draw Suns
        for (int i = 0; i < MAX_SUNS; i++) {
            if (suns[i].active) {
                const int sunSpriteSize = sunSprite.width;
                int sunSize = (suns[i].isBigSun) ? sunSpriteSize : (int)(sunSpriteSize*0.75f);
                Vector2 sunHalfSize = {sunSize/2, sunSize/2};
                suns[i].pos.y += 0.25f;
                DrawTexturePro(sunSprite, (Rectangle){0,0,sunSpriteSize, sunSpriteSize}, (Rectangle){EXPAND_V2(suns[i].pos), sunSize, sunSize }, sunHalfSize, GetTime(), WHITE);

                // Handle sun being clicked on
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    Rectangle sunBox = {EXPAND_V2(Vector2Subtract(suns[i].pos, sunHalfSize)), sunSize, sunSize};
                    if (CheckCollisionPointRec(GetMousePosition(), sunBox)) {
                        suns[i].active = false;
                        sunsCollectedCount += (suns[i].isBigSun) ? 2 : 1;
                    }
                }
            }
        }

        // Tick cooldown and spawn sun
        sunCooldown--;
        if (sunCooldown <= 0) {
            sunCooldown = 60*10;
            if (nextSun == MAX_SUNS) {
                nextSun = 0;
            }

            suns[nextSun].active = true;
            suns[nextSun].isBigSun = GetRandomValue(0, 2) == 0;
            suns[nextSun].pos.x = GetRandomValue(64, 640-64);
            suns[nextSun].pos.y = -128;

            nextSun++;
        }
        
        UpdateDrawSeedPackets();

        // Draw UI
        char sunCountText[32];
        sprintf(sunCountText, "Suns: %i", sunsCollectedCount);
        DrawText(sunCountText, 10, 10, 20, WHITE);

        EndDrawing();

    }

    CloseWindow();

    return 0;
}

void UpdateDrawPShooter(Plant* p, Vector2 screenPos)
{
    screenPos = Vector2Add(screenPos, (Vector2){10, 5});
    p->cooldown--;
    if (p->cooldown <= 0) {
        p->cooldown = 120;
        if (nextProjectile == MAX_PROJ) {
            nextProjectile = 0;
        }
        projectiles[nextProjectile].active = true;
        projectiles[nextProjectile].pos = Vector2Add(screenPos, (Vector2){33, 10});
        nextProjectile++;
    }
    DrawTexture(pShooterSprite, screenPos.x, screenPos.y, WHITE);

}

void UpdateDrawSeedPackets()
{
    int margin = 4;
    Rectangle seedTray = {seedPackets[0].origin.x-margin, seedPackets[0].origin.y-margin, SEEDPACKET_SIZE.x*8+margin*2, SEEDPACKET_SIZE.y+margin*2};
    DrawRectangleRec(seedTray, DARKBROWN);
    for (int i = 0; i < 2; i++) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !seedPackets[i].dragging) {
            Vector2 mPos = GetMousePosition();

            if (CheckCollisionPointRec(mPos, (Rectangle){seedPackets[i].origin.x, seedPackets[i].origin.y, SEEDPACKET_SIZE.x, SEEDPACKET_SIZE.y})) {
                seedPackets[i].dragging = true;
                HideCursor();
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (seedPackets[i].dragging) {
                Vector2 mpos = GetMousePosition();
                float x = (mpos.x - gridDrawOffset.x) / gridCellSize.x;
                float y = (mpos.y - gridDrawOffset.y) / gridCellSize.y;

                if (x >= 0 && y >= 0
                    && x < 9 && y < 5) {
                    gardenGrid[(int)x][(int)y].type = seedPackets[i].type;
                }
            }
            seedPackets[i].dragging = false;
            ShowCursor();
        }

        Vector2 seedPacketUIPos;

        if (seedPackets[i].dragging) {
            seedPacketUIPos = Vector2Subtract(GetMousePosition(), (Vector2){SEEDPACKET_SIZE.x/2, SEEDPACKET_SIZE.y/2});
        } else {
            seedPacketUIPos = seedPackets[i].origin;
        }
        DrawTextureV(seedPacketSprite, seedPacketUIPos, WHITE);
        switch (seedPackets[i].type) {
            case PT_PSHOOTER:
                DrawTextureEx(pShooterSprite, Vector2Add(seedPacketUIPos, (Vector2){11, 12}), 0, 0.5f, WHITE);
                break;
            default:
                break;
        }
    }
}
