#include "../include/enemy.h"
#include <math.h>
#include <stdbool.h>

// Initialisation des ennemis en fonction des plateformes
void initEnemies(Enemy enemies[], int* numEnemies, SDL_Rect platforms[], int numPlatforms) {
    *numEnemies = 3;

    // Position X de départ souhaitée pour chaque ennemi
    int positions[] = {220, 420, 650};  // Adapté pour être sur les plateformes définies

    for (int i = 0; i < *numEnemies; i++) {
        enemies[i].rect.w = 40;
        enemies[i].rect.h = 40;
        enemies[i].rect.x = positions[i];
        enemies[i].alive = 1;
        enemies[i].movingRight = (i % 2 == 0);
        enemies[i].velocity = 2;

        // Trouver la plateforme directement en dessous du centre de l'ennemi
        for (int j = 0; j < numPlatforms; j++) {
            SDL_Rect p = platforms[j];

            // Vérifie si le centre X de l'ennemi est sur la plateforme
            int enemyCenterX = positions[i] + enemies[i].rect.w / 2;
            if (enemyCenterX >= p.x && enemyCenterX <= p.x + p.w) {
                // Place l'ennemi juste au-dessus de la plateforme
                enemies[i].rect.y = p.y - enemies[i].rect.h;
                break;
            }
        }
    }
}

// Déplacement des ennemis
void moveEnemies(Enemy enemies[], int numEnemies) {
    for (int i = 0; i < numEnemies; i++) {
        if (enemies[i].alive) {
            if (enemies[i].movingRight) {
                enemies[i].rect.x += enemies[i].velocity;
            } else {
                enemies[i].rect.x -= enemies[i].velocity;
            }

            // Change de direction s'ils touchent un bord de l'écran
            if (enemies[i].rect.x <= 0 || enemies[i].rect.x >= SCREEN_WIDTH - enemies[i].rect.w) {
                enemies[i].movingRight = !enemies[i].movingRight;
            }
        }
    }
}

// Affichage des ennemis
void renderEnemies(SDL_Renderer* renderer, Enemy enemies[], int numEnemies) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge pour les ennemis
    for (int i = 0; i < numEnemies; i++) {
        if (enemies[i].alive) {
            SDL_RenderFillRect(renderer, &enemies[i].rect);
        }
    }
}

// Gestion des collisions (si nécessaire à compléter ailleurs)
void handleEnemyCollisions(Player* player, Enemy enemies[], int numEnemies);
