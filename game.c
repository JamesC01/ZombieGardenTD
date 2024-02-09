#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "Plants Vs Zombies Clone");

    SetTargetFPS(60);

    Vector2 drawPos = { 100, 100};

    int gardenGrid[9][5];
    Vector2 gridDrawOffset = {40, 80};
    int gridCellGap = 4;
    Vector2 gridCellSize = {55, 70};

    Vector2 seedPacketOrigin = {50, 10};
    Vector2 seedPacketSize = {50, 50};
    bool draggingSeedPacket = false;

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !draggingSeedPacket) {
            Vector2 mPos = GetMousePosition();

            if (CheckCollisionPointRec(mPos, (Rectangle){seedPacketOrigin.x, seedPacketOrigin.y, seedPacketSize.x, seedPacketSize.y})) {
                draggingSeedPacket = true;
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            draggingSeedPacket = false;
        }

        BeginDrawing();

        ClearBackground(WHITE);

        for (int x = 0; x < 9; x++) {
            for (int y = 0; y < 5; y++) {
                DrawRectangleV(Vector2Add(gridDrawOffset, (Vector2){x*gridCellSize.x+x*gridCellGap, y*gridCellSize.y+y*gridCellGap}), gridCellSize, RED);

            }
        }

        if (draggingSeedPacket) {
            DrawRectangleV(Vector2Subtract(GetMousePosition(), (Vector2){seedPacketSize.x/2, seedPacketSize.y/2}), seedPacketSize, BLUE);
        } else {
            DrawRectangleV(seedPacketOrigin, seedPacketSize, BLUE);
        }

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
