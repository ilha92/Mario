#ifndef COLLISION_H
#define COLLISION_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "enemy.h" // Pour avoir la structure Enemy

// Fonction de vérification de collisions
bool checkCollision(SDL_Rect a, SDL_Rect b);

// Collisions avec le sol et les plateformes
void handleCollisions(SDL_Rect* playerRect, float* velocityY, bool* isOnGround, bool* jumping, SDL_Rect ground, SDL_Rect* platforms, int numPlatforms);

// Collisions avec les ennemis
// Déclaration uniquement ici, la définition se trouve dans collision.c
void handleEnemyCollisions(Player* player, Enemy enemies[], int numEnemies);

#endif