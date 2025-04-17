#include "collision.h" // Chemin relatif simplifié, car ils sont dans le même dossier

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

void handleCollisions(SDL_Rect* playerRect, float* velocityY, bool* isOnGround, bool* jumping,
                      SDL_Rect ground, SDL_Rect* platforms, int numPlatforms) {
    *isOnGround = false;
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
            }
        }
    }
}
