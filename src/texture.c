#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <SDL2/SDL.h>

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Erreur chargement image %s : %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        printf("Erreur création texture %s : %s\n", path, SDL_GetError());
    }
    return texture;
}