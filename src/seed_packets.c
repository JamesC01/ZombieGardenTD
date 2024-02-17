#include "seed_packets.h"
#include "globals.h"
#include "assets.h"
#include <raymath.h>

SeedPacket seedPackets[SEEDPACKET_COUNT];
bool draggingSeedPacket = false;
const Vector2 SEEDPACKET_SIZE = {40, 50};

void CreateSeedPackets()
{
    // Implementation detail, the shovel is also a seedpacket. It just works.
    // TODO: get rid of hardcoded positions (set the other settings first, then loop over to set positions.)
    seedPackets[0] = (SeedPacket){ PT_NONE, (Vector2){100, 10}, 0};
    seedPackets[1] = (SeedPacket){ PT_SUNFLOWER, (Vector2){100 + SEEDPACKET_SIZE.x + 8, 10}, SUN_VALUE*2, 0, SEEDPACKET_COOLDOWN_FAST};
    seedPackets[2] = (SeedPacket){ PT_PSHOOTER, (Vector2){100 + SEEDPACKET_SIZE.x*2 + 8*2, 10}, SUN_VALUE*4, 0, SEEDPACKET_COOLDOWN_FAST};
    seedPackets[3] = (SeedPacket){ PT_WALLNUT, (Vector2){100 + SEEDPACKET_SIZE.x*3 + 8*3, 10}, SUN_VALUE*2, 0, SEEDPACKET_COOLDOWN_SLOW};
    seedPackets[4] = (SeedPacket){ PT_CHERRYBOMB, (Vector2){100 + SEEDPACKET_SIZE.x*4 + 8*4, 10}, SUN_VALUE*6, 0, SEEDPACKET_COOLDOWN_SLOW};
}

void UpdateSeedPackets()
{
    for (int i = 0; i < SEEDPACKET_COUNT; i++) {
        // Set packet dragging true if mouse clicked inside it.
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !seedPackets[i].dragging) {
            Vector2 mPos = GetMousePosition();

            if (CheckCollisionPointRec(mPos, (Rectangle){seedPackets[i].origin.x, seedPackets[i].origin.y, SEEDPACKET_SIZE.x, SEEDPACKET_SIZE.y})) {
                seedPackets[i].dragging = true;
                draggingSeedPacket = true;
                HideCursor();
            }
        }

        // Handle dropping the seed packet
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (seedPackets[i].dragging && sunsCollectedCount >= seedPackets[i].cost && seedPackets[i].buyCooldown <= 0) {
                Vector2 mpos = GetMousePosition();
                float x = (mpos.x - gridDrawOffset.x) / gridCellSize.x;
                float y = (mpos.y - gridDrawOffset.y) / gridCellSize.y;

                if (x >= 0 && y >= 0
                        && x < 9 && y < 5) {
                    // TODO: put gardengrid(x, y) into a variable
                    if (gardenGrid[(int)x][(int)y].type == PT_NONE) {
                        gardenGrid[(int)x][(int)y].type = seedPackets[i].type;
                        gardenGrid[(int)x][(int)y].cooldown = plantCooldownLUT[seedPackets[i].type];
                        gardenGrid[(int)x][(int)y].health = plantHealthLUT[seedPackets[i].type];
                        sunsCollectedCount -= seedPackets[i].cost;
                        if (seedPackets[i].type != PT_NONE) {
                            // TODO: consider making a lookup table where the PlantType is the index
                            seedPackets[i].buyCooldown = seedPackets[i].buyCooldownMax;
                        }
                    } else if (seedPackets[i].type == PT_NONE) {
                        gardenGrid[(int)x][(int)y].type = PT_NONE;
                    }
                }
            }
            seedPackets[i].dragging = false;
            draggingSeedPacket = false;
            ShowCursor();
        }
    }
}

void DrawSeedPackets()
{
    for (int i = 0; i < SEEDPACKET_COUNT; i++) {
        // Draw seedpacket
        Vector2 seedPacketUIPos;
        Texture2D shovelOrSeedPacket;

        if (seedPackets[i].type == PT_NONE) {
            shovelOrSeedPacket = shovelSprite;
        } else {
            shovelOrSeedPacket = seedPacketSprite;
        }

        if (seedPackets[i].dragging) {
            seedPacketUIPos = Vector2Subtract(GetMousePosition(), (Vector2){SEEDPACKET_SIZE.x/2, SEEDPACKET_SIZE.y/2});
            // TODO: Drawing here and also below, is this a bug?
            DrawTextureV(shovelOrSeedPacket, Vector2Add(seedPacketUIPos, (Vector2){4, 4}), (Color){0, 0, 0, 50});
        } else {
            seedPacketUIPos = seedPackets[i].origin;
        }

        DrawTextureV(shovelOrSeedPacket, seedPacketUIPos, WHITE);

        switch (seedPackets[i].type) {
            case PT_PSHOOTER:
                DrawTextureEx(pShooterSprite, Vector2Add(seedPacketUIPos, (Vector2){11, 12}), 0, 0.5f, WHITE);
                break;
            case PT_SUNFLOWER:
                DrawTextureEx(sunflowerSprite, Vector2Add(seedPacketUIPos, (Vector2){5, 12}), 0, 0.75f, WHITE);
                break;
            case PT_WALLNUT:
                DrawTextureEx(wallnutSprite, Vector2Add(seedPacketUIPos, (Vector2){5, 13}), 0, 0.7f, WHITE);
                break;
            case PT_CHERRYBOMB:
                DrawTextureEx(wallnutSprite, Vector2Add(seedPacketUIPos, (Vector2){5, 13}), 0, 0.7f, RED);
                break;
            default:
                break;
        }

        // Handle dimming seed
        if (sunsCollectedCount < seedPackets[i].cost) {
            DrawRectangleV(seedPacketUIPos, (Vector2){seedPacketSprite.width, seedPacketSprite.height}, (Color){80, 80, 80, 150});
        }
        if (seedPackets[i].buyCooldown > 0 && seedPackets[i].type != PT_NONE) {
            seedPackets[i].buyCooldown--;
            Rectangle overlayRect = (Rectangle){EXPAND_V2(seedPacketUIPos), SEEDPACKET_SIZE.x, SEEDPACKET_SIZE.y*((float)seedPackets[i].buyCooldown/(float)seedPackets[i].buyCooldownMax)};
            DrawRectangleRec(overlayRect, (Color){50, 50, 50, 100});
        }
    }
}
