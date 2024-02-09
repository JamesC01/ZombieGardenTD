#include <raylib.h>

int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "Plants Vs Zombies Clone");

    SetTargetFPS(60);

    Vector2 drawPos = { 100, 100};

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            drawPos = GetMousePosition();
        }

        BeginDrawing();

        ClearBackground(WHITE);

        DrawCircleV(drawPos, 32, (Color){204, 100, 25, 255});

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
