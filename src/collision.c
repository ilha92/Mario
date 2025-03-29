#include "collision.h"  // Inclure le fichier d'en-tête

// Fonction qui vérifie si deux rectangles se chevauchent (collisions)
int checkCollision(int x, int y, int w, int h, SDL_Rect obstacle) {
    SDL_Rect playerRect = {x, y, w, h};

    if (playerRect.x + playerRect.w > obstacle.x && playerRect.x < obstacle.x + obstacle.w &&
        playerRect.y + playerRect.h > obstacle.y && playerRect.y < obstacle.y + obstacle.h) {
        return 1; // Collision
    }
    return 0; // Pas de collision
}
