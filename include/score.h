#ifndef SCORE_H
#define SCORE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Déclaration de la fonction pour afficher le score
void displayScore(SDL_Renderer *renderer, TTF_Font *font, int score);

#endif // SCORE_H