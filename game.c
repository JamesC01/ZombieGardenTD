#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>

#define EXPAND_V2(v2) v2.x, v2.y

typedef enum {
    PT_NONE,
    PT_PSHOOTER,
    PT_SUNFLOWER,
    PT_WALLNUT,
    PT_CHERRYBOMB,
    PT_COUNT
} PlantType;

// Cooldowns apply to certain actions, for peashooters, it's the shoot frequency,
// for sunflowers, is the spawnrate of the suns.
int plantCooldownLUT[] = {
    0, // PT_NONE,
    60*2, // PT_PSHOOTER,
    60*15, // PT_SUNFLOWER,
    //PT_WALLNUT,
    //PT_CHERRYBOMB,
    //PT_COUNT
};

typedef struct {
    PlantType type;
    Vector2 origin;
    int cost;
    int buyCooldown;
    int buyCooldownMax;
    bool dragging;
} SeedPacket;
const Vector2 SEEDPACKET_SIZE = {40, 50};

typedef struct {
    PlantType type;
    int cooldown;
    float health;
} Plant;

typedef struct {
    Vector2 pos;
    bool active;
} Projectile;

typedef struct {
    Vector2 pos;
    bool active;
} Sun;

typedef struct {
    Vector2 gridPos;
    float health;
    bool active;
} Zombie;


void UpdateDrawPShooter(Plant* p, Vector2 screenPos);
void UpdateDrawSunflower(Plant* p, Vector2 screenPos);
void DrawSeedPackets();
void UpdateSeedPackets();
void SpawnSun(Vector2 pos);

// Textures
Texture2D seedPacketSprite;
Texture2D lawnBackgroundSprite;
Texture2D pShooterSprite;
Texture2D peaSprite;
Texture2D sunSprite;
Texture2D shovelSprite;
Texture2D sunflowerSprite;
Texture2D zombieSprite;

// Sounds
#define ZOMBIE_GROWL_SOUND_COUNT 4
#define ZOMBIE_HIT_SOUND_COUNT 4
Sound zombieGrowlSounds[ZOMBIE_GROWL_SOUND_COUNT];
Sound zombieHitSounds[ZOMBIE_HIT_SOUND_COUNT];

Sound peaShootSound;
Sound popSound;
Sound sunAppearSound;

// Projectile globals
#define MAX_PROJ 16
Projectile projectiles[MAX_PROJ] = {0};
int nextProjectile = 0;

// Plant Grid globals
#define GRID_WIDTH 9
#define GRID_HEIGHT 5
Plant gardenGrid[GRID_WIDTH][GRID_HEIGHT] = {0};
Vector2 gridDrawOffset = {40, 80};
int gridCellGap = 0;
Vector2 gridCellSize = {55, 70};

// Seed Packet globals
#define SEEDPACKET_COOLDOWN_SLOW 60*20
#define SEEDPACKET_COOLDOWN_NORMAL 60*15
#define SEEDPACKET_COOLDOWN_FAST 60*10
#define SEEDPACKET_COUNT 3
SeedPacket seedPackets[SEEDPACKET_COUNT];
bool draggingSeedPacket = false;

// Sun globals
#define SUN_VALUE 25
#define SUN_SPAWN_TIME 60*15
#define MAX_SUNS 8
Sun suns[MAX_SUNS] = {0};
int nextSun = 0;

int sunCooldown = 60;
int sunsCollectedCount = SUN_VALUE*2; // in PvZ, you start out with enough sun to buy a sunflower

// Zombie globals
#define ZOMBIE_SPAWN_TIME 60*20
#define MAX_ZOMBIES 16
Zombie zombies[MAX_ZOMBIES] = {0};
int nextZombie = 0;

int zombieSpawnCooldown = 60*15;

int zombieGrowlCooldown = 60*2;


int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "Plants Vs Zombies Clone");

    InitAudioDevice();

    SetTargetFPS(60);

    seedPacketSprite = LoadTexture("sprites/seedpacket.png");
    lawnBackgroundSprite = LoadTexture("sprites/lawn.png");
    pShooterSprite = LoadTexture("sprites/pshooter.png");
    peaSprite = LoadTexture("sprites/pea.png");
    sunSprite = LoadTexture("sprites/sun.png");
    shovelSprite = LoadTexture("sprites/shovel.png");
    sunflowerSprite = LoadTexture("sprites/sunflower.png");
    zombieSprite = LoadTexture("sprites/zombie.png");

    peaShootSound = LoadSound("sounds/shoot_pea.wav");
    popSound = LoadSound("sounds/pop.wav");
    sunAppearSound = LoadSound("sounds/sun_appear.wav");

    for (int i = 0; i < ZOMBIE_HIT_SOUND_COUNT; i++) {
        char path[64];
        sprintf(path, "sounds/zombie_hit%i.wav", i+1);
        zombieHitSounds[i] = LoadSound(path);
    }

    for (int i = 0; i < ZOMBIE_GROWL_SOUND_COUNT; i++) {
        char path[64];
        sprintf(path, "sounds/zombie_growl%i.wav", i+1);
        zombieGrowlSounds[i] = LoadSound(path);
    }

    // Implementation detail, the shovel is also a seedpacket. It just works.
    // TODO: get rid of hardcoded positions
    seedPackets[0] = (SeedPacket){ PT_NONE, (Vector2){100, 10}, 0};
    seedPackets[1] = (SeedPacket){ PT_SUNFLOWER, (Vector2){100 + SEEDPACKET_SIZE.x + 8, 10}, SUN_VALUE*2, 0, SEEDPACKET_COOLDOWN_FAST};
    seedPackets[2] = (SeedPacket){ PT_PSHOOTER, (Vector2){100 + SEEDPACKET_SIZE.x*2 + 8*2, 10}, SUN_VALUE*4, 0, SEEDPACKET_COOLDOWN_FAST};


    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(WHITE);

        // Draw background
        int tilesX = screenWidth/32;
        int tilesY = screenHeight/32;

        for (int x = 0; x < tilesX; x++) {
            for (int y = 0; y < tilesY; y++) {
                DrawTexture(lawnBackgroundSprite, x * 32, y * 32, WHITE);
            }
        }

        // Draw lawn grid
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {

                // Checkerboard tint
                Color c = {230, 255, 230, 50};
                bool xEven = (x % 2) == 0;
                bool yEven = (y % 2) == 0;
                if ((xEven && yEven) || (!xEven && !yEven)) {
                    c = (Color){230, 255, 230, 20};
                }

                Vector2 gridCellPos = Vector2Add(gridDrawOffset, (Vector2){x*gridCellSize.x+x*gridCellGap, y*gridCellSize.y+y*gridCellGap});

                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && draggingSeedPacket) {
                    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){gridCellPos.x, gridCellPos.y, gridCellSize.x, gridCellSize.y})) {
                        c = (Color){255, 255, 255, 100};
                    }
                }
                DrawRectangleV(gridCellPos, gridCellSize, c);
            }
        }

        // Update and Draw plants
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                if (gardenGrid[x][y].type != PT_NONE) {
                    Vector2 screenPos = Vector2Add(gridDrawOffset, (Vector2){x*gridCellSize.x+x*gridCellGap, y*gridCellSize.y+y*gridCellGap});
                    Plant* p = &gardenGrid[x][y];

                    if (p->health <= 0) {
                        p->type = PT_NONE;
                    }

                    switch (p->type) {
                        case PT_PSHOOTER:
                            UpdateDrawPShooter(p, screenPos);
                            break;
                        case PT_SUNFLOWER:
                            UpdateDrawSunflower(p, screenPos);
                        default:
                            continue;
                    }
                }
            }
        }

        // Update and draw Projectiles
        for (int i = 0; i < MAX_PROJ; i++) {
            if (projectiles[i].active) {
                projectiles[i].pos.x += 5;
                DrawTextureV(peaSprite, projectiles[i].pos, WHITE);

                if (projectiles[i].pos.x > 640) {
                    projectiles[i].active = false;
                }
            }
        }



        // Draw top bar
        DrawRectangle(0, 0, screenWidth, SEEDPACKET_SIZE.y+20, DARKBROWN);
        
        // Draw seed tray
        int margin = 4;
        Vector2 trayStart = {seedPackets[0].origin.x-margin, seedPackets[0].origin.y-margin};
        Vector2 trayEnd = {trayStart.x+8*SEEDPACKET_SIZE.x+margin, seedPackets[0].origin.y+SEEDPACKET_SIZE.y};
        DrawRectangleV(trayStart, trayEnd, (Color){46, 40, 34, 255});

        // Spawn zombies
        zombieSpawnCooldown--;
        if (zombieSpawnCooldown <= 0) {
            zombieSpawnCooldown = ZOMBIE_SPAWN_TIME;
            Vector2 gridPos = {12, GetRandomValue(0, 4)};

            // TODO: This is a duplicate of the sun spawning code. Consider refactoring.
            if (nextZombie == MAX_ZOMBIES) {
                nextZombie = 0;
            }

            zombies[nextZombie].active = true;
            zombies[nextZombie].health = 1.0f;
            zombies[nextZombie].gridPos = gridPos;

            nextZombie++;
        }

        zombieGrowlCooldown--;
        // Update and draw zombies
        for (int i = 0; i < MAX_ZOMBIES; i++) {
            if (zombies[i].active) {
                if (zombieGrowlCooldown < 0) {
                    PlaySound(zombieGrowlSounds[GetRandomValue(0, ZOMBIE_GROWL_SOUND_COUNT-1)]);
                    zombieGrowlCooldown = GetRandomValue(60*10, 60*15);
                }


                if (zombies[i].gridPos.x > 0 && zombies[i].gridPos.x < GRID_WIDTH) {
                    int frontOfZombieRounded = (int)roundf(zombies[i].gridPos.x)-1;
                    if (gardenGrid[frontOfZombieRounded][(int)zombies[i].gridPos.y].type != PT_NONE) {
                        gardenGrid[frontOfZombieRounded][(int)zombies[i].gridPos.y].health -= 0.005f;
                    } else {
                        zombies[i].gridPos.x -= 0.003f;
                    }

                } else {
                    zombies[i].gridPos.x -= 0.003f;
                }


                int x = gridDrawOffset.x + zombies[i].gridPos.x*gridCellSize.x;
                int y = gridDrawOffset.y + zombies[i].gridPos.y*gridCellSize.y;
                Rectangle box = {x-zombieSprite.width/2, y-16, zombieSprite.width, zombieSprite.height};

                for (int j = 0; j < MAX_PROJ; j++) {
                    if (projectiles[j].active) {
                        if (CheckCollisionPointRec(projectiles[j].pos, box)) {
                            projectiles[j].active = false;
                            zombies[i].health -= 0.1f;
                            PlaySound(popSound);
                            PlaySound(zombieHitSounds[GetRandomValue(0, ZOMBIE_HIT_SOUND_COUNT-1)]);
                        }
                    } 
                }

                if (zombies[i].health <= 0) {
                    zombies[i].active = false;
                }

                Rectangle src = {0,0,zombieSprite.width, zombieSprite.height};
                Rectangle dst = {x-zombieSprite.width/2, y-16, zombieSprite.width, zombieSprite.height};
                DrawTexturePro(zombieSprite, src, dst, Vector2Zero(), 0, WHITE);

                //DrawRectangleRec(box, (Color){100, 100, 255, 100});
            }
        }


        UpdateSeedPackets();
        DrawSeedPackets();

        // Spawn suns
        sunCooldown--;
        if (sunCooldown <= 0) {
            sunCooldown = SUN_SPAWN_TIME;
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
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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

        // Draw sun count
        char sunCountText[32];
        sprintf(sunCountText, "Sun:\n\n%i", sunsCollectedCount);
        DrawText(sunCountText, 32, 10, 20, WHITE);

        EndDrawing();

    }

    // TODO: Properly unload assets

    CloseWindow();

    return 0;
}

void SpawnSun(Vector2 pos)
{
    if (nextSun == MAX_SUNS) {
        nextSun = 0;
    }

    suns[nextSun].active = true;
    suns[nextSun].pos = pos;
    PlaySound(sunAppearSound);

    nextSun++;
}

void UpdateDrawSunflower(Plant* p, Vector2 screenPos)
{
    screenPos = Vector2Add(screenPos, (Vector2){8, 20});
    Vector2 sunSpawnPos = Vector2Add(screenPos, (Vector2){15, 12});
    p->cooldown--;
    if (p->cooldown <= 0) {
        p->cooldown = plantCooldownLUT[PT_SUNFLOWER];
        SpawnSun(sunSpawnPos);
    }
    DrawTexture(sunflowerSprite, screenPos.x, screenPos.y, WHITE);
}

void UpdateDrawPShooter(Plant* p, Vector2 screenPos)
{
    screenPos = Vector2Add(screenPos, (Vector2){10, 5});
    p->cooldown--;
    if (p->cooldown <= 0) {
        p->cooldown = plantCooldownLUT[PT_PSHOOTER];
        if (nextProjectile == MAX_PROJ) {
            nextProjectile = 0;
        }
        projectiles[nextProjectile].active = true;
        projectiles[nextProjectile].pos = Vector2Add(screenPos, (Vector2){33, 10});
        nextProjectile++;
        PlaySound(peaShootSound);
    }
    DrawTexture(pShooterSprite, screenPos.x, screenPos.y, WHITE);

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
                    gardenGrid[(int)x][(int)y].type = seedPackets[i].type;
                    gardenGrid[(int)x][(int)y].cooldown = plantCooldownLUT[seedPackets[i].type];
                    gardenGrid[(int)x][(int)y].health = 1.0f;
                    sunsCollectedCount -= seedPackets[i].cost;
                    if (seedPackets[i].type != PT_NONE) {
                        // TODO: consider making a lookup table where the PlantType is the index
                        seedPackets[i].buyCooldown = seedPackets[i].buyCooldownMax;
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
