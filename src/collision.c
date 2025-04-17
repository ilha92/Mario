#include "collision.h" // Chemin relatif simplifié, car ils sont dans le même dossier
<<<<<<< HEAD
#include <SDL2/SDL.h>
=======
>>>>>>> 171483704c1faaa65c40ff41fa76f969020e9cd2

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

void handleCollisions(SDL_Rect* playerRect, float* velocityY, bool* isOnGround, bool* jumping,
                      SDL_Rect ground, SDL_Rect* platforms, int numPlatforms) {
    *isOnGround = false;
<<<<<<< HEAD
    *velocityY += 0.5f; // Applique une gravité
    playerRect->y += (int)(*velocityY); // Mise à jour de la position Y du joueur en fonction de la vitesse

    // Gestion des collisions avec le sol
    if (checkCollision(*playerRect, ground)) {
        playerRect->y = ground.y - playerRect->h; // Le joueur se positionne juste au-dessus du sol
        *velocityY = 0; // La vitesse verticale est réinitialisée
        *isOnGround = true; // Le joueur est au sol
        *jumping = false; // Le joueur n'est plus en train de sauter
    }

    // Gestion des collisions avec les plateformes
    for (int i = 0; i < numPlatforms; i++) {
        SDL_Rect p = platforms[i];
        if (checkCollision(*playerRect, p)) {
            // Collision par-dessus
            if (*velocityY >= 0 && playerRect->y + playerRect->h <= p.y + 10) {
                playerRect->y = p.y - playerRect->h; // Le joueur est positionné juste au-dessus de la plateforme
                *velocityY = 0; // La vitesse verticale est réinitialisée
                *isOnGround = true; // Le joueur est sur la plateforme
                *jumping = false; // Le joueur n'est plus en train de sauter
            }
            // Collision par-dessous
            else if (*velocityY < 0 && playerRect->y >= p.y + p.h - 10) {
                playerRect->y = p.y + p.h; // Le joueur est positionné juste en dessous de la plateforme
                *velocityY = 1; // Le joueur rebondit légèrement
=======
    *velocityY += 0.5f;
    playerRect->y += (int)(*velocityY);

    // Sol
    if (checkCollision(*playerRect, ground)) {
        playerRect->y = ground.y - playerRect->h;
        *velocityY = 0;
        *isOnGround = true;
        *jumping = false;
    }

    // Plateformes
    for (int i = 0; i < numPlatforms; i++) {
        SDL_Rect p = platforms[i];
        if (checkCollision(*playerRect, p)) {
            // Par-dessus
            if (*velocityY >= 0 && playerRect->y + playerRect->h <= p.y + 10) {
                playerRect->y = p.y - playerRect->h;
                *velocityY = 0;
                *isOnGround = true;
                *jumping = false;
            }
            // Par-dessous
            else if (*velocityY < 0 && playerRect->y >= p.y + p.h - 10) {
                playerRect->y = p.y + p.h;
                *velocityY = 1;
>>>>>>> 171483704c1faaa65c40ff41fa76f969020e9cd2
            }
        }
    }
}
