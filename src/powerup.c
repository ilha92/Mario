#include "../include/powerup.h"
 // Inclure la déclaration de loadTexture
#include "../include/texture.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

// Définition des textures globales
SDL_Texture* starTexture = NULL;
SDL_Texture* mushroomTexture = NULL;

void initPowerUps(PowerUp powerUps[], int* numPowerUps, SDL_Rect platforms[], int numPlatforms) {
    *numPowerUps = 2; // Exemple : 2 power-ups

    // Position des power-ups sur les plateformes
    powerUps[0].rect = (SDL_Rect){220, 400, 30, 30}; // Étoile
    powerUps[0].type = STAR;
    powerUps[0].collected = false;

    powerUps[1].rect = (SDL_Rect){600, 250, 30, 30}; // Champignon
    powerUps[1].type = MUSHROOM;
    powerUps[1].collected = false;
}

void handlePowerUpCollection(SDL_Rect* playerRect, PowerUp powerUps[], int numPowerUps, bool* isInvincible, Uint32* invincibilityStartTime, SDL_Rect* playerRectSize, bool* isBig, Uint32* bigStartTime) {
    for (int i = 0; i < numPowerUps; i++) {
        if (!powerUps[i].collected && SDL_HasIntersection(playerRect, &powerUps[i].rect)) {
            powerUps[i].collected = true;

            if (powerUps[i].type == STAR) {
                // Activer l'invincibilité
                *isInvincible = true;
                *invincibilityStartTime = SDL_GetTicks();
                printf("Étoile collectée ! Invincibilité activée.\n");
            } else if (powerUps[i].type == MUSHROOM) {
                // Augmenter la taille du joueur
                playerRectSize->w = (int)(playerRectSize->w * 1.5);
                playerRectSize->h = (int)(playerRectSize->h * 1.5);
                *isBig = true; // Activer l'état "grand"
                *bigStartTime = SDL_GetTicks(); // Enregistrer le temps de début
                printf("Champignon collecté ! Taille augmentée.\n");
            }
        }
    }

    // Réinitialiser la taille du joueur après 15 secondes
    if (*isBig && SDL_GetTicks() - *bigStartTime > 15000) { // 15 secondes
        playerRectSize->w = 50; // Taille normale
        playerRectSize->h = 50; // Taille normale
        *isBig = false; // Désactiver l'état "grand"
        printf("L'effet du champignon est terminé. Taille normale restaurée.\n");
    }

    // Désactiver l'invincibilité après 10 secondes
    if (*isInvincible && SDL_GetTicks() - *invincibilityStartTime > 10000) { // 10 secondes
        *isInvincible = false; // Désactiver l'invincibilité
        printf("L'effet de l'étoile est terminé. Invincibilité désactivée.\n");
    }
}

void loadPowerUpTextures(SDL_Renderer* renderer) {
    starTexture = loadTexture("Images/objet/etoile.png", renderer);
    if (!starTexture) {
        printf("Erreur : Impossible de charger l'image etoile.png\n");
    }


    mushroomTexture = loadTexture("Images/objet/champignon.png", renderer);
    if (!mushroomTexture) {
        printf("Erreur : Impossible de charger l'image champignon.png\n");
    }

    if (!starTexture || !mushroomTexture) {
        printf("Erreur chargement des textures des power-ups.\n");
        exit(1);
    }
}

void renderPowerUps(SDL_Renderer* renderer, PowerUp powerUps[], int numPowerUps) {
    for (int i = 0; i < numPowerUps; i++) {
        if (!powerUps[i].collected) {
            SDL_Texture* texture = (powerUps[i].type == STAR) ? starTexture : mushroomTexture;
            SDL_RenderCopy(renderer, texture, NULL, &powerUps[i].rect);
        }
    }
}

void cleanUpPowerUpTextures() {
    if (starTexture) SDL_DestroyTexture(starTexture);
    if (mushroomTexture) SDL_DestroyTexture(mushroomTexture);
}

bool initIMG() {
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur IMG_Init : %s\n", IMG_GetError());
        return false;
    }
    return true;
}