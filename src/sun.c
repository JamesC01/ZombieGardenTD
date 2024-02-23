#include "sun.h"
#include "seed_packets.h"
#include "assets.h"
#include <string.h>
#include "game.h"
#include <raymath.h>

FixedObjectArray suns;

int sunCooldown;
int sunsCollectedCount;// in PvZ, you start out with enough sun to buy a sunflower
                                      //
void InitSuns(void)
{
    // Init sun variables
    Sun* sunArr = (Sun*)suns.array;
    for (int i = 0; i < suns.fixedSize; i++) {
        sunArr[i].active = false;
    }

    sunCooldown = 60;
    sunsCollectedCount = 9999;//SUN_VALUE*2; // in PvZ, you start out with enough sun to buy a sunflower
}

void SpawnSun(Vector2 pos)
{
    Sun* sunArr = (Sun*)suns.array;
    sunArr[suns.next].active = true;
    sunArr[suns.next].pos = pos;
    PlaySound(sunAppearSound);

    NextObject(&suns);
}

void UpdateDrawSuns(void)
{
    // Spawn suns
    if (TickCooldown(&sunCooldown, SUN_SPAWN_TIME)) {
        Vector2 pos = {GetRandomValue(64, screenWidth-64), SEEDPACKET_SIZE.y+32};
        SpawnSun(pos);
    }

    // Update and draw Suns
    Sun *sunArr = (Sun*)suns.array;
    for (int i = 0; i < suns.fixedSize; i++) {
        if (sunArr[i].active) {
            sunArr[i].pos.y += 0.25f;

            const int sunSize = sunSprite.width;
            Vector2 sunHalfSize = {(float)sunSize/2, (float)sunSize/2};

            // Handle sun being clicked on
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !draggingSeedPacket) {
                Rectangle sunBox = {EXPAND_V2(Vector2Subtract(sunArr[i].pos, sunHalfSize)), sunSize, sunSize};
                if (CheckCollisionPointRec(GetMousePosition(), sunBox)) {
                    sunArr[i].active = false;
                    sunsCollectedCount += SUN_VALUE;
                }
            }

            // Draw sun
            Rectangle src = {0,0,sunSize, sunSize};
            Rectangle dst = { EXPAND_V2(sunArr[i].pos), sunSize, sunSize};
            DrawTexturePro(sunSprite, src, dst, sunHalfSize, GetTime()*10, WHITE);
        }
    }
}
