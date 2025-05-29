#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void displayScore(SDL_Renderer *renderer, TTF_Font *font, int score) {
    SDL_Color color = {255, 255, 255, 255}; // Couleur blanches
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_Rect scoreRect = {10, 10, 100, 50}; // Position en haut à gauches
    SDL_RenderCopy(renderer, texture, NULL, &scoreRect);
    SDL_DestroyTexture(texture);
}