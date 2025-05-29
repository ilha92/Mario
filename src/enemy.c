#include "../include/enemy.h"
#include <math.h>
#include <stdbool.h>

void initEnemies(Enemy enemies[], int* numEnemies, SDL_Rect platforms[], int numPlatforms) {
    *numEnemies = 3;

    // Position X de départ souhaitée pour chaque ennemi
    int positions[] = {220, 420, 650}; 

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

void handleEnemyCollisions(Player* player, Enemy enemies[], int numEnemies, bool isInvincible) {
    for (int i = 0; i < numEnemies; i++) {
        if (!enemies[i].alive || !player->alive) continue;

        // Vérifier la collision entre le joueur et l'ennemi
        if (SDL_HasIntersection(&player->rect, &enemies[i].rect)) {
            if (isInvincible) {
                // Si le joueur est invincible, il ne perd pas de vie
                printf("Collision avec un ennemi, mais le joueur est invincible.\n");
                continue; // Ignorer cette collision
            }

            // Si le joueur n'est pas invincible, il perd une vie
            player->health = 0;  // Le joueur perd toute sa santé
            checkPlayerLives(player);  // Vérifier les vies restantes

            // Si le joueur est encore en vie, respawn au point de départ
            if (player->alive) {
                printf("Respawn au point de départ.\n");
                player->rect.x = 100;  // Position de départ
                player->rect.y = 100;
            }
        }
    }
}

void updateEnemies(Enemy enemies[], int numEnemies, SDL_Rect* playerRect, Player* player, int* score, 
                   SDL_Rect platforms[], int numPlatforms, float* velocityY, bool isInvincible) {
    Uint32 now = SDL_GetTicks();
    for (int i = 0; i < numEnemies; i++) {
        if (enemies[i].alive) {
            // Déplacement de l'ennemi
            if (enemies[i].movingRight) {
                enemies[i].rect.x += enemies[i].velocity;
            } else {
                enemies[i].rect.x -= enemies[i].velocity;
            }

            // Vérification des bords de la plateforme
            int platformIndex = enemies[i].platformIndex;
            SDL_Rect p = platforms[platformIndex];
            if (enemies[i].rect.x <= p.x || enemies[i].rect.x + enemies[i].rect.w >= p.x + p.w) {
                enemies[i].movingRight = !enemies[i].movingRight;
            }

            // Collision avec le joueur
            if (SDL_HasIntersection(playerRect, &enemies[i].rect)) {
                // Si le joueur est invincible, on ignore la collision
                if (isInvincible) {
                    continue;
                }
                if (playerRect->y + playerRect->h - 10 < enemies[i].rect.y) {
                    // Le joueur saute sur l'ennemi → tue l'ennemi
                    enemies[i].alive = 0;
                    enemies[i].deathTime = now;
                    *score += 100;
                    *velocityY = -10;
                    printf("Ennemi %d tué !\n", i);
                } else {
                    // Le joueur touche l'ennemi de côté → il perd une vie
                    if (player->lives > 0) {
                        player->lives--;
                        printf("Il vous reste %d vies.\n", player->lives);
                        if (player->lives <= 0) {
                            player->alive = false;
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
                enemies[i].movingRight = true;
                printf("Ennemi %d respawn à la position initiale %d.\n", i, enemies[i].initialX);
            }
        }
    }
}
