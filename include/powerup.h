#ifndef POWERUP_H
#define POWERUP_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef enum {
    STAR,       // Étoile d'invincibilité
    MUSHROOM    // Champignon pour augmenter la taille
} PowerUpType;

typedef struct {
    SDL_Rect rect;       // Position et taille du power-up
    PowerUpType type;    // Type de power-up
    bool collected;      // Indique si le power-up a été collecté
} PowerUp;

void initPowerUps(PowerUp powerUps[], int* numPowerUps, SDL_Rect platforms[], int numPlatforms);
void handlePowerUpCollection(SDL_Rect* playerRect, PowerUp powerUps[], int numPowerUps, bool* isInvincible, Uint32* invincibilityStartTime, SDL_Rect* playerRectSize, bool* isBig, Uint32* bigStartTime);
void loadPowerUpTextures(SDL_Renderer* renderer);
void renderPowerUps(SDL_Renderer* renderer, PowerUp powerUps[], int numPowerUps);
void cleanUpPowerUpTextures();

#endif
