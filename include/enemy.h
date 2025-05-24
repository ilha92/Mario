#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include "player.h"
#include <stdbool.h>

#define MAX_ENEMIES 10
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct {
    SDL_Rect rect;     // Position et taille de l'ennemi
    int velocity;      // Vitesse de déplacement
    int movingRight;   // 1 = va à droite, 0 = va à gauche
    bool alive;        // Indique si l'ennemi est vivant
    Uint32 deathTime;  // Temps de la mort pour le respawn
    int initialX;      // Position initiale en X
    int platformIndex; // Indice de la plateforme associée
} Enemy;

// Initialise les ennemis
void initEnemies(Enemy enemies[], int* numEnemies, SDL_Rect platforms[], int numPlatforms);

// Déplace les ennemis (droite/gauche)
void moveEnemies(Enemy enemies[], int numEnemies, SDL_Rect platforms[], int numPlatforms);

// Dessine les ennemis
void renderEnemies(SDL_Renderer* renderer, Enemy enemies[], int numEnemies);

// Gère les collisions entre le joueur et les ennemis
void handleEnemyCollisions(Player* player, Enemy enemies[], int numEnemies);

// Met à jour les ennemis, vérifie les collisions et gère le score
void updateEnemies(Enemy enemies[], int numEnemies, SDL_Rect* playerRect, int* playerLives, int* score, SDL_Rect platforms[], int numPlatforms, float* velocityY);

#endif
