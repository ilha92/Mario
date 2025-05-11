#include "collision.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>   // Ajout pour fabsf
#include "enemy.h"

// Fonction pour vérifier les collisions entre deux rectangles
bool checkCollision(SDL_Rect rect1, SDL_Rect rect2) {
    return (rect1.x + rect1.w > rect2.x && rect1.x < rect2.x + rect2.w &&
            rect1.y + rect1.h > rect2.y && rect1.y < rect2.y + rect2.h);
}

// Fonction de gestion des collisions
void handleCollisions(SDL_Rect* playerRect, float* velocityY, bool* isOnGround, bool* jumping,
                      SDL_Rect ground, SDL_Rect* platforms, int numPlatforms) {
    *isOnGround = false;

    // Collision avec le sol
    if (checkCollision(*playerRect, ground)) {
        playerRect->y = ground.y - playerRect->h;  // Mettre le joueur sur le sol
        *velocityY = 0;  // Stopper la vitesse verticale
        *isOnGround = true;  // Le joueur est au sol
        *jumping = false;  // Plus en train de sauter
    }

    // Collision avec les plateformes
    for (int i = 0; i < numPlatforms; i++) {
        SDL_Rect platform = platforms[i];

        // Si le joueur tombe (vitesseY > 0) et entre en collision avec la plateforme par le bas
        if (checkCollision(*playerRect, platform) && *velocityY > 0) {
            playerRect->y = platform.y - playerRect->h;  // Positionner le joueur juste sur la plateforme
            *velocityY = 0;  // Stopper la descente
            *isOnGround = true;  // Le joueur est sur la plateforme
            *jumping = false;  // Il n'est plus en train de sauter
        }

        // Si le joueur se déplace vers le haut (vitesseY < 0) et entre en collision avec la plateforme par le haut
        if (checkCollision(*playerRect, platform) && *velocityY < 0) {
            playerRect->y = platform.y + platform.h;  // Positionner le joueur juste sous la plateforme
            *velocityY = 0;  // Stopper le mouvement vertical (la plateforme bloque le saut)
            *jumping = false;  // Le joueur n'est plus en train de sauter
        }
    }
}

void handleEnemyCollisions(Player* player, Enemy enemies[], int numEnemies) {
    for (int i = 0; i < numEnemies; i++) {
        if (!enemies[i].alive || !player->alive) continue;

        // Vérifier la collision entre le joueur et l'ennemi
        if (SDL_HasIntersection(&player->rect, &enemies[i].rect)) {
            bool hitFromTop = (player->rect.y + player->rect.h - 5) <= enemies[i].rect.y;
            bool falling = player->velocityY >= 0;  // Le joueur tombe

            if (hitFromTop && falling) {
                // Mario saute sur l'ennemi → tue l'ennemi
                enemies[i].alive = 0; // L'ennemi est tué
                player->velocityY = -8; // Le joueur rebondit
            } else {
                // Collision latérale ou par dessous -> Le joueur perd une vie
                player->lives--;  // Le joueur perd une vie
                printf("Vous avez %d vies restantes.\n", player->lives);

                // Si le joueur n'a plus de vies, il est mort
                if (player->lives <= 0) {
                    player->alive = 0;  // Le joueur est mort
                    printf("Game Over! Vous avez perdu toutes vos vies.\n");
                    return;  // Quitte la fonction si le joueur est mort
                }

                // Réinitialiser la position du joueur
                player->rect.x = 100;
                player->rect.y = 100;
                player->velocityY = 0; // Bloquer aussi verticalement si besoin
            }
        }
    }
}