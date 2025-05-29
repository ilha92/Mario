#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);

#endif