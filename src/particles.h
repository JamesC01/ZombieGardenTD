#ifndef PARTICLES_H
#define PARTICLES_H

#include "game.h"
#include <raylib.h>

typedef enum {
    P_DEFAULT,
    P_RAIN
} ParticleType;

typedef struct {
    ParticleType type;
    Vector2 pos;
    Vector2 size;
    Vector2 velocity;
    Color colour;
    int startLifetime;
    int lifetime;
    bool active;
} Particle;

extern FixedObjectArray particles;

Particle *CreateParticle(ParticleType type, Vector2 pos, Vector2 size, int lifetime, Color colour);
void CreateParticleConfetti(Vector2 pos, Vector2 size, int particleCount);
void CreateParticleExplosion(Vector2 pos, Vector2 size, int maxSpeed, int lifetime, int particleCount, Color colour);
void UpdateDrawParticles(void);

#endif
