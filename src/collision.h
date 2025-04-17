#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

<<<<<<< HEAD
void handleCollisions(SDL_Rect *playerRect, float *velocityY, bool *isOnGround, bool *jumping, SDL_Rect ground, SDL_Rect *platforms, int numPlatforms);
=======
bool checkCollision(SDL_Rect a, SDL_Rect b);

void handleCollisions(SDL_Rect* playerRect, float* velocityY, bool* isOnGround, bool* jumping,
                      SDL_Rect ground, SDL_Rect* platforms, int numPlatforms);
>>>>>>> 171483704c1faaa65c40ff41fa76f969020e9cd2

bool checkCollision(SDL_Rect a, SDL_Rect b);

void handleCollisions(SDL_Rect* playerRect, float* velocityY, bool* isOnGround, bool* jumping,
                      SDL_Rect ground, SDL_Rect* platforms, int numPlatforms);

#endif