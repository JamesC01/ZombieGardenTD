#include "particles.h"
#include <raymath.h>
#include <stdlib.h>
#include "game.h"

FixedObjectArray particles;

// TODO: Consider adding minSpeed
void CreateParticleExplosion(Vector2 pos, Vector2 size, int maxSpeed, int lifetime, int particleCount, Color colour)
{
    Particle *partArr = (Particle*)particles.array;
    for (int i = 0; i < particleCount; i++) {
        Particle* p = &partArr[particles.next];


        p->pos = pos;
        p->size = size;
        Vector2 randDir = Vector2Normalize((Vector2){GetRandomFloatValue(-1, 1), GetRandomFloatValue(-1, 1)});
        p->velocity = Vector2Scale(randDir, GetRandomFloatValue(0, maxSpeed));
        p->colour = colour;
        p->startLifetime = lifetime;
        p->lifetime = lifetime;
        p->active = true;


        NextObject(&particles);
    }
}

void UpdateDrawParticles(void)
{
    Particle *partArr = (Particle*)particles.array;
    for (int i = 0; i < particles.fixedSize; i++) {
        Particle *p = &partArr[i];
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
