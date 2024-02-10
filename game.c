#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

#define EXPAND_V2(v2) v2.x, v2.y

typedef enum {
    PT_NONE,
    PT_PSHOOTER,
    PT_WALLNUT,
    PT_CHERRYBOMB,
    PT_COUNT
} PlantType;

typedef struct {
    PlantType type;
    // packet sprite
    Vector2 origin;
    bool dragging;

} SeedPacket;
const Vector2 SEEDPACKET_SIZE = {40, 50};

typedef struct {
    PlantType type;
    int cooldown;
    float health;
} Plant;


int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "Plants Vs Zombies Clone");

    SetTargetFPS(60);

    Texture2D seedPacketSprite = LoadTexture("sprites/seedpacket.png");
    Texture2D lawnBackgroundSprite = LoadTexture("sprites/lawn.png");

    Vector2 drawPos = { 100, 100};

    Plant gardenGrid[9][5] = {0};
    Vector2 gridDrawOffset = {40, 80};
    int gridCellGap = 0;
    Vector2 gridCellSize = {55, 70};

    SeedPacket seedPacket = {0};
    seedPacket.origin = (Vector2){50, 10};

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !seedPacket.dragging) {
            Vector2 mPos = GetMousePosition();

            if (CheckCollisionPointRec(mPos, (Rectangle){seedPacket.origin.x, seedPacket.origin.y, SEEDPACKET_SIZE.x, SEEDPACKET_SIZE.y})) {
                seedPacket.dragging = true;
                HideCursor();
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (seedPacket.dragging) {
                Vector2 mpos = GetMousePosition();
                int x = (mpos.x - gridDrawOffset.x) / gridCellSize.x;
                int y = (mpos.y - gridDrawOffset.y) / gridCellSize.y;

                gardenGrid[x][y].type = PT_PSHOOTER;
            }
            seedPacket.dragging = false;
            ShowCursor();
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

                if (seedPacket.dragging) {
                    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){gridCellPos.x, gridCellPos.y, gridCellSize.x, gridCellSize.y})) {
                        c = (Color){255, 255, 255, 100};
                    }
                }
                DrawRectangleV(gridCellPos, gridCellSize, c);
            }
        }

        // Draw plants
        for (int x = 0; x < 9; x++) {
            for (int y = 0; y < 5; y++) {
                if (gardenGrid[x][y].type != PT_NONE) {
                    Vector2 gridCellPos = Vector2Add(gridDrawOffset, (Vector2){x*gridCellSize.x+x*gridCellGap, y*gridCellSize.y+y*gridCellGap});
                    DrawRectangleV(gridCellPos, gridCellSize, BLUE);
                }
            }
        }


        // Draw dragged seed packet
        if (seedPacket.dragging) {
            DrawTextureV(seedPacketSprite, Vector2Subtract(GetMousePosition(), (Vector2){SEEDPACKET_SIZE.x/2, SEEDPACKET_SIZE.y/2}), WHITE);
        } else {
            DrawTextureV(seedPacketSprite, seedPacket.origin, WHITE);
        }

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
