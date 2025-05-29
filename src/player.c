#include <stdbool.h> 
#include "player.h"
#include "config.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h> // Pour les calculs trigonométriques

// Fonction qui initialise le joueur
void initPlayer(Player* player) {
    player->rect.x = 100;  // Position de départ en X
    player->rect.y = 100;  // Position de départ en Y
    player->rect.w = 50;   // Largeur du joueur
    player->rect.h = 50;   // Hauteur du joueur
    player->velocityX = 0;
    player->velocityY = 0;
    player->isOnGround = 1;
    player->jumping = 0;
    player->facingRight = 1;
    player->alive = 1;
    player->health = 100;
    player->lives = 3; // Initialiser le nombre de vies
}

// Fonction pour gérer la perte de vies
void checkPlayerLives(Player* player) {
    if (player->health <= 0) {
        player->lives -= 1;  // Le joueur perd une vie
        if (player->lives > 0) {
            player->health = 100;  // Réinitialiser la santé
            player->rect.x = 100;  // Position de départ
            player->rect.y = 100;
            printf("Il vous reste %d vies.\n", player->lives);
        } else {
            player->alive = false;  // Le joueur est mort
            printf("Perdu ! Vous avez épuisé toutes vos vies.\n");
        }
    }
}

void drawHeart(SDL_Renderer* renderer, int x, int y, int size, bool filled) {
    int radius = size / 4; // Rayon des cercles
    int triangleHeight = size / 2; // Hauteur du triangle

    // Dessiner les deux cercles
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // Distance horizontale au centre
            int dy = radius - h; // Distance verticale au centre
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                if (filled) {
                    SDL_RenderDrawPoint(renderer, x - radius + dx, y - radius + dy); // Cercle gauche
                    SDL_RenderDrawPoint(renderer, x + radius + dx, y - radius + dy); // Cercle droit
                }
            }
        }
    }

    // Dessiner le triangle
    for (int i = 0; i < triangleHeight; i++) {
        int startX = x - radius + i;
        int endX = x + radius - i;
        for (int j = startX; j <= endX; j++) {
            if (filled) {
                SDL_RenderDrawPoint(renderer, j, y + i);
            }
        }
    }
}
// Fonction pour afficher les cœurs représentant les vies du joueur
void displayHeartRects(SDL_Renderer* renderer, int lives) {
    int heartSize = 30;  // Taille d'un cœur
    int spacing = 10;    // Espacement entre les cœurs
    int startX = SCREEN_WIDTH - (3 * (heartSize + spacing)); // Position de départ en X (3 cœurs maximum)
    int startY = 40;     // Position de départ en Y

    for (int i = 0; i < 3; i++) {
        int x = startX + i * (heartSize + spacing);
        int y = startY;

        if (i < lives) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge = cœur plein
            drawHeart(renderer, x, y, heartSize, true);
        } else {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gris = cœur vide
            drawHeart(renderer, x, y, heartSize, true);
        }
    }
}
// Fonction qui met à jour la position du joueur et la caméra
void update_player_and_camera(Player* player, SDL_Rect* camera) {
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_RIGHT]) {
        player->rect.x += 5;
        player->facingRight = 1;
    }
    if (state[SDL_SCANCODE_LEFT]) {
        player->rect.x -= 5;
        player->facingRight = 0;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        player->rect.y += 5;
    }
    if (state[SDL_SCANCODE_UP]) {
        player->rect.y -= 5;
    }

    // Mise à jour de la caméra
    camera->x = player->rect.x + player->rect.w/2 - SCREEN_WIDTH/2;
    camera->y = player->rect.y + player->rect.h/2 - SCREEN_HEIGHT/2;

    // Empêcher la caméra de sortir de la map
    if (camera->x < 0) camera->x = 0;
    if (camera->y < 0) camera->y = 0;
    if (camera->x > MAP_WIDTH - camera->w) camera->x = MAP_WIDTH - camera->w;
    if (camera->y > MAP_HEIGHT - camera->h) camera->y = MAP_HEIGHT - camera->h;
}

// Fonction de rendu du joueur (exemple)
void renderPlayer(SDL_Renderer* renderer, Player* player, SDL_Rect camera) {
    SDL_Rect renderQuad = {
        player->rect.x - camera.x,
        player->rect.y - camera.y,
        player->rect.w,
        player->rect.h
    };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge
    SDL_RenderFillRect(renderer, &renderQuad); // Affiche Mario à la bonne position caméra
}

// Mise à jour de la gravité et du saut (ajoutée pour une meilleure interaction)
void updateGravityAndJump(Player* player) {
    if (!player->isOnGround) {
        player->velocityY += 0.5f;  // Gravité
    } else {
        player->velocityY = 0;  // Réinitialiser la vélocité verticale lorsque sur le sol
    }

    // Application du saut
    player->rect.y += (int)player->velocityY;

    // Limite pour ne pas passer sous le sol
    if (player->rect.y > SCREEN_HEIGHT - player->rect.h) {
        player->rect.y = SCREEN_HEIGHT - player->rect.h;
        player->isOnGround = 1;
        player->jumping = 0;
    }
}

// Gestion du saut
void jump(Player* player) {
    if (player->isOnGround) {
        player->velocityY = -12;  // Saut
        player->isOnGround = 0;
        player->jumping = 1;
    }
}
