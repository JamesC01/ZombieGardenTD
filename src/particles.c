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

        particles[nextParticle].pos = pos;
        particles[nextParticle].size = size;
        Vector2 randDir = Vector2Normalize((Vector2){(float)rand()/(float)RAND_MAX-0.5f, (float)rand()/(float)RAND_MAX-0.5f});
        particles[nextParticle].velocity = Vector2Scale(randDir, GetRandomValue(0, maxSpeed-1)+(float)rand()/(float)RAND_MAX);
        particles[nextParticle].colour = colour;
        particles[nextParticle].lifetime = lifetime;
        particles[nextParticle].active = true;


        NextObject(&nextParticle, MAX_PARTICLES);
    }
}

void UpdateDrawParticles(void)
{
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].pos = Vector2Add(particles[i].pos, particles[i].velocity);
            particles[i].lifetime--;

            DrawRectangleV(particles[i].pos, particles[i].size, particles[i].colour);

            if (particles[i].lifetime <= 0) {
                particles[i].active = false;
            }
        }
    }
}
