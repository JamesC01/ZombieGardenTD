#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

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

int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "Plants Vs Zombies Clone");

    SetTargetFPS(60);

    Texture2D seedPacketSprite = LoadTexture("sprites/seedpacket.png");

    Vector2 drawPos = { 100, 100};

    int gardenGrid[9][5];
    Vector2 gridDrawOffset = {40, 80};
    int gridCellGap = 4;
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
            seedPacket.dragging = false;
            ShowCursor();
        }

        BeginDrawing();

        ClearBackground(WHITE);

        for (int x = 0; x < 9; x++) {
            for (int y = 0; y < 5; y++) {
                Vector2 gridCellPos = Vector2Add(gridDrawOffset, (Vector2){x*gridCellSize.x+x*gridCellGap, y*gridCellSize.y+y*gridCellGap});
                Color c = RED;
                if (seedPacket.dragging) {
                    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){gridCellPos.x, gridCellPos.y, gridCellSize.x, gridCellSize.y})) {
                        c = GREEN;
                    }
                }
                DrawRectangleV(gridCellPos, gridCellSize, c);
            }
        }

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
