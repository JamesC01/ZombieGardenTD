#ifndef PARTICLES_H
#define PARTICLES_H

#include <raylib.h>

typedef struct {
    Vector2 pos;
    Vector2 size;
    Vector2 velocity;
    Color colour;
    int lifetime;
    bool active;
} Particle;

// Particle globals
#define MAX_PARTICLES 128
extern Particle particles[MAX_PARTICLES];
extern int nextParticle;

void CreateParticleExplosion(Vector2 pos, Vector2 size, int maxSpeed, int lifetime, int particleCount, Color colour);
void UpdateDrawParticles(void);

#endif
