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
    seedPackets[0] = (SeedPacket){ PT_NONE, Vector2Zero()};
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
        SeedPacket* packet = &seedPackets[i];
        Vector2 mPos = GetMousePosVirtual();
        // Set packet dragging true if mouse clicked inside it.
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !packet->dragging) {
            if (CheckCollisionPointRec(mPos, (Rectangle){packet->origin.x, packet->origin.y, SEEDPACKET_SIZE.x, SEEDPACKET_SIZE.y})) {
                packet->dragging = true;
                draggingSeedPacket = true;
                HideCursor();
            }
        }

        // Handle dropping the seed packet
        else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && packet->dragging) {
            bool canBuySeedPacket = sunsCollectedCount >= packet->cost && packet->buyCooldown <= 0;
            if (canBuySeedPacket) {
                float gridX = (mPos.x - gridDrawOffset.x) / gridCellSize.x;
                float gridY = (mPos.y - gridDrawOffset.y) / gridCellSize.y;

                bool insideGrid = gridX >= 0 && gridY >= 0 && gridX < GRID_WIDTH && gridY < GRID_HEIGHT;
                if (insideGrid) {

                    Plant* plant = &gardenGrid[(int)gridX][(int)gridY];

                    bool noPlantInCell = plant->type == PT_NONE;
                    bool usingShovel = packet->type == PT_NONE;

                    if (noPlantInCell) {
                        // Plant the plant
                        plant->type = packet->type;
                        plant->cooldown = plantCooldownLUT[packet->type];
                        plant->health = plantHealthLUT[packet->type];
                        sunsCollectedCount -= packet->cost;

                        if (!usingShovel) {
                            packet->buyCooldown = packet->buyCooldownMax;
                        }
                    } else if (usingShovel) {
                        plant->type = PT_NONE;
                    }
                }
            }

            packet->dragging = false;
            draggingSeedPacket = false;
            ShowCursor();
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && packet->dragging) {
            packet->dragging = false;
            draggingSeedPacket = false;
        }
    }
}

void DrawSeedPackets()
{
    for (int i = 0; i < SEEDPACKET_COUNT; i++) {
        Vector2 seedPacketUIPos;
        Texture2D currentSprite;

        bool usingShovel = seedPackets[i].type == PT_NONE;

        currentSprite = (usingShovel) ? shovelSprite : seedPacketSprite;

        if (seedPackets[i].dragging) {
            seedPacketUIPos = Vector2Subtract(GetMousePosVirtual(), (Vector2){SEEDPACKET_SIZE.x/2, SEEDPACKET_SIZE.y/2});
        } else {
            seedPacketUIPos = seedPackets[i].origin;
        }

        DrawTextureV(currentSprite, seedPacketUIPos, WHITE);

        // Highlighting seedpacket
        Rectangle bounds = {EXPAND_V2(seedPacketUIPos), EXPAND_V2(SEEDPACKET_SIZE)};
        if (CheckCollisionPointRec(GetMousePosVirtual(), bounds) && !draggingSeedPacket && seedPackets[i].type != PT_NONE) {
            DrawRectangleV(seedPacketUIPos, SEEDPACKET_SIZE, (Color){255, 255, 255, 50});
        }

        Vector2 seedPacketMiddle = {seedPacketUIPos.x+SEEDPACKET_SIZE.x/2, seedPacketUIPos.y+SEEDPACKET_SIZE.y*0.575f};

        // Draw plant icon
        switch (seedPackets[i].type) {
            case PT_PSHOOTER:
                DrawTextureCentered(pShooterSprite, seedPacketMiddle, GetTextureCenterPoint(pShooterSprite), WHITE, 0.45f);
                break;
            case PT_SUNFLOWER:
                DrawTextureCentered(sunflowerSprite, seedPacketMiddle, GetTextureCenterPoint(sunflowerSprite), WHITE, 0.6f);
                break;
            case PT_WALLNUT:
                DrawTextureCentered(wallnutSprite, seedPacketMiddle, GetTextureCenterPoint(wallnutSprite), WHITE, 0.6f);
                break;
            case PT_CHERRYBOMB:
                DrawTextureCentered(cherrySprite, seedPacketMiddle, GetTextureCenterPoint(cherrySprite), WHITE, 0.6f);
                break;
            default:
                break;
        }

        // Dim seed packet
        bool canAffordPacket = sunsCollectedCount >= seedPackets[i].cost;
        if (!canAffordPacket) {
            DrawRectangleV(seedPacketUIPos, (Vector2){seedPacketSprite.width, seedPacketSprite.height}, (Color){80, 80, 80, 150});
        }

        // Draw cooldown overlay
        if (seedPackets[i].buyCooldown > 0 && seedPackets[i].type != PT_NONE) {
            seedPackets[i].buyCooldown--;

            Rectangle cooldownRect = {
                EXPAND_V2(seedPacketUIPos),
                SEEDPACKET_SIZE.x,
                SEEDPACKET_SIZE.y*((float)seedPackets[i].buyCooldown/(float)seedPackets[i].buyCooldownMax)
            };
            DrawRectangleRec(cooldownRect, (Color){50, 50, 50, 100});
        }
    }
}
