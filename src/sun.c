#include "sun.h"
#include "particles.h"
#include "seed_packets.h"
#include "assets.h"
#include <raylib.h>
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
    sunsCollectedCount = SUN_VALUE*2; // in PvZ, you start out with enough sun to buy a sunflower
}

void SpawnSun(Vector2 pos)
{
    Sun* sun = GetNextObject(suns, Sun);
    IncrementArrayIndex(&suns);

    sun->active = true;
    sun->pos = pos;

    PlaySound(sunAppearSound);
}

void UpdateDrawSuns(void)
{
    // Spawn suns
    if (TickCooldown(&sunCooldown, SUN_SPAWN_TIME)) {
        Vector2 pos = {GetRandomValue(64, virtualScreenWidth-64), SEEDPACKET_SIZE.y+32};
        SpawnSun(pos);
    }

    // Update and draw Suns
    Sun *sunArr = (Sun*)suns.array;
    for (int i = 0; i < suns.fixedSize; i++) {
        if (sunArr[i].active) {
            sunArr[i].pos.y += 0.25f;

            Vector2 sunOrigin = GetTextureCenterPoint(sunSprite);
            Vector2 sunTopLeftPos = Vector2Subtract(sunArr[i].pos, sunOrigin);

            Rectangle sunBox = {sunTopLeftPos.x, sunTopLeftPos.y, sunSprite.width, sunSprite.height};

            if (CheckCollisionPointRec(GetMousePosVirtual(), sunBox)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !draggingSeedPacket) {
                    sunArr[i].active = false;
                    sunsCollectedCount += SUN_VALUE;
                } else if (!draggingSeedPacket) {
                    CreateParticleExplosion(sunArr[i].pos, (Vector2){4, 4}, 4, 10, 2, WHITE);
                }
            }

            DrawTextureFull(sunSprite, sunArr[i].pos, sunOrigin, WHITE, 1, GetTime()*20);
        }
    }
}
