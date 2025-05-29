#include "../include/enemy.h"
#include <math.h> 
#include <stdbool.h>

void initEnemies(Enemy enemies[], int* numEnemies) {
    *numEnemies = 3;

    enemies[0].rect = (SDL_Rect){200, 400, 40, 40};
    enemies[0].alive = 1;
    enemies[0].movingRight = 1;
    enemies[0].velocity = 2;

    enemies[1].rect = (SDL_Rect){400, 400, 40, 40};
    enemies[1].alive = 1;
    enemies[1].movingRight = 0;
    enemies[1].velocity = 2;

    enemies[2].rect = (SDL_Rect){600, 400, 40, 40};
    enemies[2].alive = 1;
    enemies[2].movingRight = 1;
    enemies[2].velocity = 2;
}

void moveEnemies(Enemy enemies[], int numEnemies) {
    for (int i = 0; i < numEnemies; i++) {
        if (enemies[i].alive) {
            if (enemies[i].movingRight) {
                enemies[i].rect.x += enemies[i].velocity;
            } else {
                enemies[i].rect.x -= enemies[i].velocity;
            }

            // Change de direction s'ils touchent un bord de l'écrans
            if (enemies[i].rect.x <= 0 || enemies[i].rect.x >= SCREEN_WIDTH - enemies[i].rect.w) {
                enemies[i].movingRight = !enemies[i].movingRight;
            }
        }
    }
}

void renderEnemies(SDL_Renderer* renderer, Enemy enemies[], int numEnemies) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge pour les ennemis
    for (int i = 0; i < numEnemies; i++) {
        if (enemies[i].alive) {
            SDL_RenderFillRect(renderer, &enemies[i].rect);
        }
    }
}
void handleEnemyCollisions(Player* player, Enemy enemies[], int numEnemies);
