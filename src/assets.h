#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"

// Textures
extern Texture2D seedPacketSprite;
extern Texture2D lawnBackgroundSprite;
extern Texture2D pShooterSprite;
extern Texture2D peaSprite;
extern Texture2D sunSprite;
extern Texture2D shovelSprite;
extern Texture2D sunflowerSprite;
extern Texture2D wallnutSprite;
extern Texture2D cherrySprite;
extern Texture2D zombieSprite;
extern Texture2D shadowSprite;
extern Texture2D smallShadowSprite;

// TODO: Consider making a function that takes in a sprite and returns the center point
#define SHADOW_ORIGIN (Vector2){(float)shadowSprite.width/2, (float)shadowSprite.height/2}

// Sounds
#define ZOMBIE_GROWL_SOUND_COUNT 4
#define ZOMBIE_HIT_SOUND_COUNT 4
extern Sound zombieGrowlSounds[ZOMBIE_GROWL_SOUND_COUNT];
extern Sound zombieHitSounds[ZOMBIE_HIT_SOUND_COUNT];

extern Sound peaShootSound;
extern Sound popSound;
extern Sound sunAppearSound;

extern Music themeSong;

void LoadAssets();
void UnloadAssets();

#endif
