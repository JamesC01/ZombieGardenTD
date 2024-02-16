#include "assets.h"
#include <stdio.h>

Texture2D seedPacketSprite;
Texture2D lawnBackgroundSprite;
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

void LoadAssets()
{
    seedPacketSprite = LoadTexture("sprites/seedpacket.png");
    lawnBackgroundSprite = LoadTexture("sprites/lawn.png");
    pShooterSprite = LoadTexture("sprites/pshooter.png");
    peaSprite = LoadTexture("sprites/pea.png");
    sunSprite = LoadTexture("sprites/sun.png");
    shovelSprite = LoadTexture("sprites/shovel.png");
    sunflowerSprite = LoadTexture("sprites/sunflower.png");
    wallnutSprite = LoadTexture("sprites/wallnut.png");
    cherrySprite = LoadTexture("sprites/wallnut.png");
    zombieSprite = LoadTexture("sprites/zombie.png");
    shadowSprite = LoadTexture("sprites/shadow.png");
    smallShadowSprite = LoadTexture("sprites/small_shadow.png");

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
}
