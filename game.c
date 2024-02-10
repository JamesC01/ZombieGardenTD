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


void UpdateDrawPShooter(Plant* p, Vector2 screenPos);
void UpdateDrawSeedPackets();

// Assets
Texture2D seedPacketSprite;
Texture2D lawnBackgroundSprite;
Texture2D pShooterSprite;
Texture2D peaSprite;

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

    seedPackets[0] = (SeedPacket){ PT_NONE, (Vector2){50, 10}};
    seedPackets[1] = (SeedPacket){ PT_PSHOOTER, (Vector2){50 + SEEDPACKET_SIZE.x, 10}};

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
        for (int i = 0; i < 16; i++) {
            if (projectiles[i].active) {
                projectiles[i].pos.x += 5;
                DrawTextureV(peaSprite, projectiles[i].pos, WHITE);
            }
        }


        UpdateDrawSeedPackets();

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

        if (seedPackets[i].dragging) {
            DrawTextureV(seedPacketSprite, Vector2Subtract(GetMousePosition(), (Vector2){SEEDPACKET_SIZE.x/2, SEEDPACKET_SIZE.y/2}), WHITE);
        } else {
            DrawTextureV(seedPacketSprite, seedPackets[i].origin, WHITE);
        }
    }
}
