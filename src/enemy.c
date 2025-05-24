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
        enemies[i].velocity = 1;
        enemies[i].initialX = positions[i]; // Enregistre la position initiale

        // Trouver la plateforme directement en dessous du centre de l'ennemi
        for (int j = 0; j < numPlatforms; j++) {
            SDL_Rect p = platforms[j];

            // Vérifie si le centre X de l'ennemi est sur la plateforme
            int enemyCenterX = positions[i] + enemies[i].rect.w / 2;
            if (enemyCenterX >= p.x && enemyCenterX <= p.x + p.w) {
                // Place l'ennemi juste au-dessus de la plateforme
                enemies[i].rect.y = p.y - enemies[i].rect.h;
                enemies[i].platformIndex = j; // Enregistre l'indice de la plateforme
                break;
            }
        }
    }
}

void moveEnemies(Enemy enemies[], int numEnemies, SDL_Rect platforms[], int numPlatforms) {
    Uint32 now = SDL_GetTicks();
    for (int i = 0; i < numEnemies; i++) {
        if (enemies[i].alive) {
            // Déplacement de l'ennemi
            if (enemies[i].movingRight) {
                enemies[i].rect.x += enemies[i].velocity;
            } else {
                enemies[i].rect.x -= enemies[i].velocity;
            }

            // Vérifier si l'ennemi atteint les bords de la plateforme
            int platformIndex = enemies[i].platformIndex;
            SDL_Rect p = platforms[platformIndex];

            if (enemies[i].rect.x <= p.x || enemies[i].rect.x + enemies[i].rect.w >= p.x + p.w) {
                enemies[i].movingRight = !enemies[i].movingRight; // Change de direction
            }
        } else {
            // Si l'ennemi est mort, vérifier le temps pour respawn
            if (now - enemies[i].deathTime >= 10000) { // 10 secondes
                printf("Respawn de l'ennemi %d\n", i);
                printf("Position initiale : %d, Plateforme : %d\n", enemies[i].initialX, enemies[i].platformIndex);
                enemies[i].alive = 1;
                enemies[i].rect.x = enemies[i].initialX;
                int platformIndex = enemies[i].platformIndex;
                SDL_Rect p = platforms[platformIndex];
                enemies[i].rect.y = p.y - enemies[i].rect.h;
                enemies[i].movingRight = 1;
                enemies[i].velocity = 1;
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

// enemy.c
void updateEnemies(Enemy enemies[], int numEnemies, SDL_Rect* playerRect, int* playerLives, int* score, SDL_Rect platforms[], int numPlatforms, float* velocityY) {
    Uint32 now = SDL_GetTicks();

    for (int i = 0; i < numEnemies; i++) {
        if (enemies[i].alive) {
            // Déplacement de l'ennemi
            if (enemies[i].movingRight) {
                enemies[i].rect.x += enemies[i].velocity;
            } else {
                enemies[i].rect.x -= enemies[i].velocity;
            }

            // Vérifier si l'ennemi atteint les bords de la plateforme
            int platformIndex = enemies[i].platformIndex;
            SDL_Rect p = platforms[platformIndex];
            if (enemies[i].rect.x <= p.x || enemies[i].rect.x + enemies[i].rect.w >= p.x + p.w) {
                enemies[i].movingRight = !enemies[i].movingRight; // Change de direction
            }

            // Collision avec le joueur
            if (SDL_HasIntersection(playerRect, &enemies[i].rect)) {
                if (playerRect->y + playerRect->h - 10 < enemies[i].rect.y) {
                    // Le joueur saute sur l'ennemi → tue l'ennemi
                    enemies[i].alive = 0;
                    enemies[i].deathTime = now;
                    *score += 100;
                    *velocityY = -10; // Rebond du joueur
                    printf("Ennemi %d tué !\n", i);
                } else {
                    // Le joueur touche l'ennemi de côté → il perd une vie
                    if (*playerLives > 0) {
                        (*playerLives)--;
                        printf("Il vous reste %d vies.\n", *playerLives);
                        if (*playerLives <= 0) {
                            printf("Perdu ! Vous avez épuisé toutes vos vies.\n");
                        }
                    }
                    enemies[i].alive = 0;
                    enemies[i].deathTime = now;
                }
            }
        } else {
            // Respawn de l'ennemi après 10 secondes
            if (now - enemies[i].deathTime >= 10000) {
                enemies[i].alive = 1;
                enemies[i].rect.x = enemies[i].initialX;
                int platformIndex = enemies[i].platformIndex;
                SDL_Rect p = platforms[platformIndex];
                enemies[i].rect.y = p.y - enemies[i].rect.h;
                enemies[i].movingRight = 1;
                printf("Ennemi %d respawn à la position initiale %d.\n", i, enemies[i].initialX);
            }
        }
    }
}