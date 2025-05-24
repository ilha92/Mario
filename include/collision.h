#ifndef COLLISION_H
#define COLLISION_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "enemy.h" // Pour avoir la structure Enemy
#include "powerup.h" // Pour avoir la structure PowerUp

// Fonction de vérification de collision
bool checkCollision(SDL_Rect a, SDL_Rect b);

// Collisions avec le sol et les plateformes
void handleCollisions(SDL_Rect* playerRect, float* velocityY, bool* isOnGround, bool* jumping, SDL_Rect ground, SDL_Rect* platforms, int numPlatforms);

// Collisions avec les ennemis
// Déclaration uniquement ici, la définition se trouve dans collision.c
void handleEnemyCollisions(Player* player, Enemy enemies[], int numEnemies) ;

// Collisions avec les power-ups
void handlePowerUpCollisions(SDL_Rect* playerRect, PowerUp powerUps[], int numPowerUps, bool* isInvincible, Uint32* invincibilityStartTime, SDL_Rect* playerSize);
#endif
