#include "coin.h"
#include "collision.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

void handleCoinCollection(SDL_Rect* playerRect, Coin* coins, int numCoins, int* score) {
    for (int i = 0; i < numCoins; i++) {
        if (!coins[i].collected && SDL_HasIntersection(playerRect, &coins[i].rect)) {
            coins[i].collected = true;
            *score += 10;
        }
    }
}

// Fonction pour faire apparaître une nouvelle pièce
void spawnCoin(Coin coins[], int numCoins, SDL_Rect ground, SDL_Rect* platforms, int numPlatforms) {
    for (int i = 0; i < numCoins; i++) {
        if (coins[i].collected) {
            // Choisit un emplacement aléatoire sur le sol ou sur les plateformes
            int r = rand() % (numPlatforms + 1); // Nombre de plateformes + sol
            SDL_Rect target = (r == numPlatforms) ? ground : platforms[r];
            coins[i].rect.x = target.x + rand() % (target.w - coins[i].rect.w); // Position aléatoire en X
            coins[i].rect.y = target.y - coins[i].rect.h; // Position en Y au-dessus du sol ou de la plateforme
            coins[i].collected = false; // La pièce n'est plus collectée
            break;
        }
    }
}
