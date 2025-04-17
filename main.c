#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

#include "collision.h"
#include "coin.h"
#include "score.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FRAME_TIME 100

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* runMarioFrames[4];
SDL_Texture* idleMario = NULL;
SDL_Texture* coinTexture = NULL;

typedef enum { IDLE, RUN, JUMP } State;
State currentState = IDLE;

SDL_Rect playerRect = {100, 100, 50, 50};
float velocityY = 0;
bool isOnGround = false;
bool jumping = false;
bool facingRight = true;

SDL_Rect ground = {0, 550, 800, 50};
SDL_Rect platforms[2] = {
    {200, 450, 150, 20},
    {400, 380, 150, 20}
};
int numPlatforms = 2;

Coin coins[5] = {
    {{220, 430, 30, 30}, false},
    {{420, 360, 30, 30}, false},
    {{550, 430, 30, 30}, false},
    {{200, 500, 30, 30}, false},
    {{380, 330, 30, 30}, false}
};
int numCoins = 5;

int currentFrame = 0;
Uint32 lastFrameTime = 0;
int score = 0;

// Chargement d'une image avec vérification
SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Erreur chargement image %s : %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        printf("Erreur texture %s : %s\n", path, SDL_GetError());
    }
    return texture;
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init : %s\n", SDL_GetError());
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur IMG_Init : %s\n", IMG_GetError());
        return false;
    }
    if (TTF_Init() == -1) {
        printf("Erreur TTF_Init : %s\n", TTF_GetError());
        return false;
    }

    window = SDL_CreateWindow("Super Mario Platform", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void loadTextures() {
    for (int i = 0; i < 4; i++) {
        char path[64];
        snprintf(path, sizeof(path), "Images/run_mario/%d.png", i);
        runMarioFrames[i] = loadTexture(path);
    }
    idleMario = loadTexture("Images/idle_mario/0.png");
    coinTexture = loadTexture("Images/idle_coin/0.png");
}

void updateFrame() {
    if ((currentState == RUN || currentState == JUMP) && SDL_GetTicks() - lastFrameTime > FRAME_TIME) {
        currentFrame = (currentFrame + 1) % 4;
        lastFrameTime = SDL_GetTicks();
    }
}

void render(TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 92, 148, 252, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &ground);

    SDL_SetRenderDrawColor(renderer, 180, 100, 50, 255);
    for (int i = 0; i < numPlatforms; i++) {
        SDL_RenderFillRect(renderer, &platforms[i]);
    }

    // Affichage des pièces
    for (int i = 0; i < numCoins; i++) {
        if (!coins[i].collected) {
            SDL_RenderCopy(renderer, coinTexture, NULL, &coins[i].rect);
        }
    }

    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_Texture* marioTexture = (currentState == RUN || currentState == JUMP) ? runMarioFrames[currentFrame] : idleMario;
    SDL_RenderCopyEx(renderer, marioTexture, NULL, &playerRect, 0, NULL, flip);

    displayScore(renderer, font, score);
    SDL_RenderPresent(renderer);
}

void cleanUp() {
    for (int i = 0; i < 4; ++i) {
        if (runMarioFrames[i]) SDL_DestroyTexture(runMarioFrames[i]);
    }
    if (idleMario) SDL_DestroyTexture(idleMario);
    if (coinTexture) SDL_DestroyTexture(coinTexture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!init()) return -1;
    loadTextures();

    TTF_Font* font = TTF_OpenFont("Images/arial.ttf", 24);  // Assurez-vous d'avoir la police
    if (!font) {
        printf("Erreur chargement police : %s\n", TTF_GetError());
        return -1;
    }

    bool quit = false;
    SDL_Event e;
    bool moveLeft = false, moveRight = false;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: quit = true; break;
                    case SDLK_LEFT: moveLeft = true; facingRight = false; break;
                    case SDLK_RIGHT: moveRight = true; facingRight = true; break;
                    case SDLK_SPACE:
                        if (isOnGround) {
                            velocityY = -12;
                            jumping = true;
                            currentState = JUMP;
                        }
                        break;
                }
            } else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT: moveLeft = false; break;
                    case SDLK_RIGHT: moveRight = false; break;
                }
            }
        }

        if (moveLeft) playerRect.x -= 6;
        if (moveRight) playerRect.x += 6;

        if (moveLeft || moveRight)
            currentState = RUN;
        else if (!jumping)
            currentState = IDLE;

        handleCollisions(&playerRect, &velocityY, &isOnGround, &jumping, ground, platforms, numPlatforms);

        // Gère la collecte des pièces et la réapparition
        handleCoinCollection(&playerRect, coins, numCoins, &score);

        // L'appel de spawnCoin fait réapparaître les pièces collectées
        for (int i = 0; i < numCoins; i++) {
            if (coins[i].collected) {
                spawnCoin(coins, numCoins, ground, platforms, numPlatforms);  // Réapparaît la pièce
            }
        }

        updateFrame();
        render(font);
        SDL_Delay(16);
    }

    cleanUp();
    return 0;
}
