// player.h
#ifndef PLAYER_H
#define PLAYER_H
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_Rect rect;
    int velocityX;
    int velocityY;
    int isOnGround;
    int jumping;
    int facingRight;
    int health;
    int lives;
    bool alive;
} Player;

// Déclarations externes des variables globales
extern int SCREEN_WIDTH;     // Largeur de l'écran
extern int SCREEN_HEIGHT;    // Hauteur de l'écran
extern int MAP_WIDTH;        // Largeur de la carte
extern int MAP_HEIGHT;       // Hauteur de la carte

// Déclaration des fonctions
void initPlayer(Player* player);
void checkPlayerLives(Player* player);
void update_player_and_camera(Player* player, SDL_Rect* camera);
void renderPlayer(SDL_Renderer* renderer, Player* player, SDL_Rect camera);
void updateGravityAndJump(Player* player);
void jump(Player* player);
void displayHeartRects(SDL_Renderer* renderer, int lives);
#endif
