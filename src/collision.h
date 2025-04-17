#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>
#include <SDL2/SDL.h>

void handleCollisions(SDL_Rect *playerRect, float *velocityY, bool *isOnGround, bool *jumping, SDL_Rect ground, SDL_Rect *platforms, int numPlatforms);

bool checkCollision(SDL_Rect a, SDL_Rect b);

void handleCollisions(SDL_Rect* playerRect, float* velocityY, bool* isOnGround, bool* jumping,
                      SDL_Rect ground, SDL_Rect* platforms, int numPlatforms);

#endif