#ifndef PARTICLES_H
#define PARTICLES_H

#include "game.h"
#include <raylib.h>

typedef struct {
    Vector2 pos;
    Vector2 size;
    Vector2 velocity;
    Color colour;
    int startLifetime;
    int lifetime;
    bool active;
} Particle;

extern FixedObjectArray particles;

void CreateParticleConfetti(Vector2 pos, Vector2 size, int particleCount);
void CreateParticleExplosion(Vector2 pos, Vector2 size, int maxSpeed, int lifetime, int particleCount, Color colour);
void UpdateDrawParticles(void);

#endif
