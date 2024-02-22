#include "particles.h"
#include <raymath.h>
#include <stdlib.h>
#include "game.h"

Particle particles[MAX_PARTICLES] = {0};
int nextParticle = 0;

// TODO: Consider adding minSpeed
void CreateParticleExplosion(Vector2 pos, Vector2 size, int maxSpeed, int lifetime, int particleCount, Color colour)
{
    for (int i = 0; i < particleCount; i++) {
        Particle* p = &particles[nextParticle];

        p->pos = pos;
        p->size = size;
        Vector2 randDir = Vector2Normalize((Vector2){GetRandomFloatValue(-1, 1), GetRandomFloatValue(-1, 1)});
        p->velocity = Vector2Scale(randDir, GetRandomFloatValue(0, maxSpeed));
        p->colour = colour;
        p->startLifetime = lifetime;
        p->lifetime = lifetime;
        p->active = true;


        NextObject(&nextParticle, MAX_PARTICLES);
    }
}

void UpdateDrawParticles(void)
{
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &particles[i];
        if (p->active) {
            p->pos = Vector2Add(p->pos, p->velocity);
            p->lifetime--;

            //p->colour.a = 255 * (p->lifetime / p->startLifetime);

            DrawRectangleV(p->pos, p->size, p->colour);

            if (p->lifetime <= 0) {
                p->active = false;
            }
        }
    }
}
