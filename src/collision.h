#ifndef COLLISION_H
#define COLLISION_H

#include <SDL2/SDL.h>

// Déclaration de la fonction de vérification de collision
int checkCollision(int x, int y, int w, int h, SDL_Rect obstacle);

#endif
