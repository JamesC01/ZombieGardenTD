#include "seed_packets.h"
#include "game.h"
#include "assets.h"
#include "sun.h"
#include <raylib.h>
#include <raymath.h>

#define SEEDPACKET_COOLDOWN_SLOW 60*20
#define SEEDPACKET_COOLDOWN_NORMAL 60*15
#define SEEDPACKET_COOLDOWN_FAST 60*10

SeedPacket seedPackets[SEEDPACKET_COUNT];
bool draggingSeedPacket = false;
const Vector2 SEEDPACKET_SIZE = {40, 50};

void CreateSeedPackets()
{
    // Implementation detail, the shovel is also a seedpacket. It just works.
    seedPackets[0] = (SeedPacket){ PT_NONE, Vector2Zero(), 0};
    seedPackets[1] = (SeedPacket){ PT_SUNFLOWER, Vector2Zero(), SUN_VALUE*2, 0, SEEDPACKET_COOLDOWN_FAST};
    seedPackets[2] = (SeedPacket){ PT_PSHOOTER, Vector2Zero(), SUN_VALUE*4, 0, SEEDPACKET_COOLDOWN_FAST};
    seedPackets[3] = (SeedPacket){ PT_WALLNUT, Vector2Zero(), SUN_VALUE*2, 0, SEEDPACKET_COOLDOWN_SLOW};
    seedPackets[4] = (SeedPacket){ PT_CHERRYBOMB, Vector2Zero(), SUN_VALUE*6, 0, SEEDPACKET_COOLDOWN_SLOW};

    const int spacing = 8;
    const int leftEdge = 100;
    for (int i = 0; i < SEEDPACKET_COUNT; i++) {
        seedPackets[i].origin = (Vector2){leftEdge + SEEDPACKET_SIZE.x*i + spacing*i, 10};
    }
}

void UpdateDrawSeedPackets()
{
    UpdateSeedPackets();
    DrawSeedPackets();
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
        else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && seedPackets[i].dragging) {
            if (seedPackets[i].dragging && sunsCollectedCount >= seedPackets[i].cost && seedPackets[i].buyCooldown <= 0) {
                Vector2 mpos = GetMousePosition();
                float x = (mpos.x - gridDrawOffset.x) / gridCellSize.x;
                float y = (mpos.y - gridDrawOffset.y) / gridCellSize.y;

                if (x >= 0 && y >= 0
                        && x < 9 && y < 5) {

                    Plant* plant = &gardenGrid[(int)x][(int)y];
                    // Grid spot is empty
                    if (plant->type == PT_NONE) {
                        plant->type = seedPackets[i].type;
                        plant->cooldown = plantCooldownLUT[seedPackets[i].type];
                        plant->health = plantHealthLUT[seedPackets[i].type];
                        sunsCollectedCount -= seedPackets[i].cost;
                        if (seedPackets[i].type != PT_NONE) {
                            seedPackets[i].buyCooldown = seedPackets[i].buyCooldownMax;
                        }
                    } else if (seedPackets[i].type == PT_NONE) {
                        plant->type = PT_NONE;
                    }
                }
            }
            seedPackets[i].dragging = false;
            draggingSeedPacket = false;
            ShowCursor();
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && seedPackets[i].dragging) {
            seedPackets[i].dragging = false;
            draggingSeedPacket = false;
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
