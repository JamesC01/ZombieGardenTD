#include "sun.h"
#include "seed_packets.h"
#include "assets.h"
#include <string.h>
#include "game.h"
#include <raymath.h>

Sun suns[MAX_SUNS] = {0};
int nextSun = 0;

int sunCooldown;
int sunsCollectedCount;// in PvZ, you start out with enough sun to buy a sunflower
                                      //
void InitSuns(void)
{
    // Init sun variables
    memset(suns, 0, MAX_SUNS * sizeof(Sun));
    nextSun = 0;

    sunCooldown = 60;
    sunsCollectedCount = SUN_VALUE*2; // in PvZ, you start out with enough sun to buy a sunflower
}

void SpawnSun(Vector2 pos)
{
    suns[nextSun].active = true;
    suns[nextSun].pos = pos;
    PlaySound(sunAppearSound);

    NextObject(&nextSun, MAX_SUNS);
}

void UpdateDrawSuns(void)
{
    // Spawn suns
    if (TickCooldown(&sunCooldown, SUN_SPAWN_TIME)) {
        Vector2 pos = {GetRandomValue(64, screenWidth-64), SEEDPACKET_SIZE.y+32};
        SpawnSun(pos);
    }

    // Update and draw Suns
    for (int i = 0; i < MAX_SUNS; i++) {
        if (suns[i].active) {
            suns[i].pos.y += 0.25f;

            const int sunSize = sunSprite.width;
            Vector2 sunHalfSize = {(float)sunSize/2, (float)sunSize/2};

            // Handle sun being clicked on
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !draggingSeedPacket) {
                Rectangle sunBox = {EXPAND_V2(Vector2Subtract(suns[i].pos, sunHalfSize)), sunSize, sunSize};
                if (CheckCollisionPointRec(GetMousePosition(), sunBox)) {
                    suns[i].active = false;
                    sunsCollectedCount += SUN_VALUE;
                }
            }

            // Draw sun
            Rectangle src = {0,0,sunSize, sunSize};
            Rectangle dst = { EXPAND_V2(suns[i].pos), sunSize, sunSize};
            DrawTexturePro(sunSprite, src, dst, sunHalfSize, GetTime()*10, WHITE);
        }
    }
}
