#include "assets.h"
#include "zombie.h"
#include <raylib.h>
#include <stdio.h>

Texture2D seedPacketSprite;
Texture2D lawnBackgroundSprite;
Texture2D woodBackgroundSprite;
Texture2D seedSprite;
Texture2D sunSprite;
Texture2D shovelSprite;

Texture2D seedShooterSprite;
Texture2D sunflowerSprite;
Texture2D coconutSprite;
Texture2D potatoSprite;

Texture2D zombieSprite;
Texture2D zombieHeadSprite;
Texture2D headlessZombieSprite;

Texture2D shadowSprite;
Texture2D smallShadowSprite;

Texture2D seedShooterFlashSprite;
Texture2D sunflowerFlashSprite;
Texture2D coconutFlashSprite;
Texture2D potatoFlashSprite;
Texture2D zombieFlashSprite;
Texture2D headlessZombieFlashSprite;

Sound zombieGrowlSounds[ZOMBIE_GROWL_SOUND_COUNT];
Sound zombieHitSounds[ZOMBIE_HIT_SOUND_COUNT];

Sound peaShootSound;
Sound popSound;
Sound sunAppearSound;
Sound headSplatSound;
Sound digUpPlantSound;
Sound placePlantSound;
Sound explodeSound;

Font bigFont;
Font smallFont;

Music themeSong;
Music rainLoop;

Texture2D CreateFlashSprite(Texture2D sprite);

Texture2D CreateFlashSprite(Texture2D sprite)
{
    Image flashImg = LoadImageFromTexture(sprite);

    for (int x = 0; x < flashImg.width; x++) {
        for (int y = 0; y < flashImg.height; y++) {
            Color pixel = GetImageColor(flashImg, x, y);

            if (pixel.a != 0) {
                ImageDrawPixel(&flashImg, x, y, (Color){255, 220, 30, 80});
            }
        }
    }

    Texture2D flashSprite = LoadTextureFromImage(flashImg);
    UnloadImage(flashImg);

    return flashSprite;
}

void LoadAssets()
{
    seedPacketSprite = LoadTexture("sprites/seedpacket.png");
    lawnBackgroundSprite = LoadTexture("sprites/lawn.png");
    SetTextureWrap(lawnBackgroundSprite, TEXTURE_WRAP_REPEAT);
    woodBackgroundSprite = LoadTexture("sprites/wood.png");
    SetTextureWrap(woodBackgroundSprite, TEXTURE_WRAP_REPEAT);
    seedShooterSprite = LoadTexture("sprites/seedshooter.png");
    seedSprite = LoadTexture("sprites/seed.png");
    sunSprite = LoadTexture("sprites/sun.png");
    shovelSprite = LoadTexture("sprites/shovel.png");
    sunflowerSprite = LoadTexture("sprites/sunflower.png");
    coconutSprite = LoadTexture("sprites/coconut.png");
    potatoSprite = LoadTexture("sprites/potatobomb.png");
    zombieSprite = LoadTexture("sprites/zombie.png");
    zombieHeadSprite = LoadTexture("sprites/zombiehead.png");
    headlessZombieSprite = LoadTexture("sprites/headlesszombie.png");
    shadowSprite = LoadTexture("sprites/shadow.png");
    smallShadowSprite = LoadTexture("sprites/small_shadow.png");

    seedShooterFlashSprite = CreateFlashSprite(seedShooterSprite);
    sunflowerFlashSprite = CreateFlashSprite(sunflowerSprite);
    coconutFlashSprite = CreateFlashSprite(coconutSprite);
    potatoFlashSprite = CreateFlashSprite(potatoSprite);

    zombieFlashSprite = CreateFlashSprite(zombieSprite);
    headlessZombieFlashSprite = CreateFlashSprite(headlessZombieSprite);


    themeSong = LoadMusicStream("sounds/theme.mp3");
    rainLoop = LoadMusicStream("sounds/rain.mp3");

    peaShootSound = LoadSound("sounds/shoot_pea.wav");
    popSound = LoadSound("sounds/pop.wav");
    sunAppearSound = LoadSound("sounds/sun_appear.wav");
    headSplatSound = LoadSound("sounds/headsplat.wav");
    digUpPlantSound = LoadSound("sounds/digup.wav");
    placePlantSound = LoadSound("sounds/plant.wav");
    explodeSound = LoadSound("sounds/explode.wav");

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
    // TODO: I'm not freeing everything. This is a sign I need a better way to handle this. Maybe
    // keep an array of each type of asset that just holds a pointer to it, and then loop over the
    // array. I can just make a wrapper function that automatically adds it to the array.
    UnloadTexture(seedPacketSprite);
    UnloadTexture(lawnBackgroundSprite );
    UnloadTexture(seedShooterSprite);
    UnloadTexture(seedSprite);
    UnloadTexture(sunSprite);
    UnloadTexture(shovelSprite);
    UnloadTexture(sunflowerSprite);
    UnloadTexture(coconutSprite);
    UnloadTexture(potatoSprite);
    UnloadTexture(zombieSprite);
    UnloadTexture(shadowSprite);
    UnloadTexture(smallShadowSprite);

    UnloadTexture(zombieFlashSprite);

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
