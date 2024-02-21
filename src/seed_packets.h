#ifndef SEED_PACKETS_H
#define SEED_PACKETS_H
#include <raylib.h>
#include "plant.h"

typedef struct {
    PlantType type;
    Vector2 origin;
    int cost;
    int buyCooldown;
    int buyCooldownMax;
    bool dragging;
} SeedPacket;
const extern Vector2 SEEDPACKET_SIZE;

// Seed Packet globals
#define SEEDPACKET_COUNT 5 // +1 because of the shovel
extern SeedPacket seedPackets[SEEDPACKET_COUNT];
extern bool draggingSeedPacket;

void CreateSeedPackets();
void UpdateDrawSeedPackets();
void UpdateSeedPackets();
void DrawSeedPackets();

#endif
