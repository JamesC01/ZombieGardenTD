#include "assets.h"
#include <raylib.h>
#include <stdio.h>

Texture2D seedPacketSprite;
Texture2D lawnBackgroundSprite;
Texture2D woodBackgroundSprite;
Texture2D pShooterSprite;
Texture2D peaSprite;
Texture2D sunSprite;
Texture2D shovelSprite;
Texture2D sunflowerSprite;
Texture2D wallnutSprite;
Texture2D cherrySprite;
Texture2D zombieSprite;
Texture2D shadowSprite;
Texture2D smallShadowSprite;

Sound zombieGrowlSounds[ZOMBIE_GROWL_SOUND_COUNT];
Sound zombieHitSounds[ZOMBIE_HIT_SOUND_COUNT];

Sound peaShootSound;
Sound popSound;
Sound sunAppearSound;

Font bigFont;
Font smallFont;

Music themeSong;
Music rainLoop;

void LoadAssets()
{
    seedPacketSprite = LoadTexture("sprites/seedpacket.png");
    lawnBackgroundSprite = LoadTexture("sprites/lawn.png");
    SetTextureWrap(lawnBackgroundSprite, TEXTURE_WRAP_REPEAT);
    woodBackgroundSprite = LoadTexture("sprites/wood.png");
    SetTextureWrap(woodBackgroundSprite, TEXTURE_WRAP_REPEAT);
    pShooterSprite = LoadTexture("sprites/pshooter.png");
    peaSprite = LoadTexture("sprites/pea.png");
    sunSprite = LoadTexture("sprites/sun.png");
    shovelSprite = LoadTexture("sprites/shovel.png");
    sunflowerSprite = LoadTexture("sprites/sunflower.png");
    wallnutSprite = LoadTexture("sprites/wallnut.png");
    cherrySprite = LoadTexture("sprites/cherrybomb.png");
    zombieSprite = LoadTexture("sprites/zombie.png");
    shadowSprite = LoadTexture("sprites/shadow.png");
    smallShadowSprite = LoadTexture("sprites/small_shadow.png");

    themeSong = LoadMusicStream("sounds/theme.mp3");
    rainLoop = LoadMusicStream("sounds/rain.mp3");

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

    bigFont = LoadFont("big_font.ttf");
    smallFont = LoadFont("small_font.ttf");
    while (!IsFontReady(bigFont) || !IsFontReady(smallFont)) {}
}

void UnloadAssets()
{
    UnloadTexture(seedPacketSprite);
    UnloadTexture(lawnBackgroundSprite );
    UnloadTexture(pShooterSprite);
    UnloadTexture(peaSprite);
    UnloadTexture(sunSprite);
    UnloadTexture(shovelSprite);
    UnloadTexture(sunflowerSprite);
    UnloadTexture(wallnutSprite);
    UnloadTexture(cherrySprite);
    UnloadTexture(zombieSprite);
    UnloadTexture(shadowSprite);
    UnloadTexture(smallShadowSprite);

    UnloadMusicStream(themeSong);


    UnloadSound(peaShootSound);
    UnloadSound(popSound);
    UnloadSound(sunAppearSound);

    for (int i = 0; i < ZOMBIE_HIT_SOUND_COUNT; i++) {
        UnloadSound(zombieHitSounds[i]);
    }

    for (int i = 0; i < ZOMBIE_GROWL_SOUND_COUNT; i++) {
        UnloadSound(zombieGrowlSounds[i]);
    }
}
