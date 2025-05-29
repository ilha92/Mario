#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include "player.h"

#define MAX_ENEMIES 10
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct {
    SDL_Rect rect;     // Position et taille de l'ennemi
    int velocity;      // Vitesse de déplacement
    int movingRight;   // 1 = va à droite, 0 = va à gauche
    int alive;         // 1 = vivant, 0 = morts
} Enemy;

// Initialise les ennemis
void initEnemies(Enemy enemies[], int* numEnemies);

// Déplace les ennemis (droite/gauche)
void moveEnemies(Enemy enemies[], int numEnemies);

// Dessine les ennemis
void renderEnemies(SDL_Renderer* renderer, Enemy enemies[], int numEnemies);

#endif
