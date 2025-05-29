#ifndef COIN_H
#define COIN_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// Structure représentant une pièce
typedef struct {
    SDL_Rect rect;   // Rectangle pour la position et la taille de la pièce
    bool collected;  // Si la pièce a été collectée ou non
} Coin;

// Déclarations des fonctions
void handleCoinCollection(SDL_Rect *playerRect, Coin *coins, int numCoins, int *score);
void spawnCoin(Coin coins[], int numCoins, SDL_Rect ground, SDL_Rect *platforms, int numPlatforms);

#endif