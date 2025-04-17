#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "src/collision.h"

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

// Structure pour les pièces
typedef struct {
    SDL_Rect rect;
    bool collected;
} Coin;

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

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (!IMG_Init(IMG_INIT_PNG)) return false;

    window = SDL_CreateWindow("Super Mario Platform",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer != NULL;
}

SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) return NULL;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void loadTextures() {
    runMarioFrames[0] = loadTexture("Images/run_mario/0.png");
    runMarioFrames[1] = loadTexture("Images/run_mario/1.png");
    runMarioFrames[2] = loadTexture("Images/run_mario/2.png");
    runMarioFrames[3] = loadTexture("Images/run_mario/3.png");
    idleMario = loadTexture("Images/idle_mario/0.png");
    coinTexture = loadTexture("Images/idle_coin/0.png");
}

void updateFrame() {
    if (currentState == RUN && SDL_GetTicks() - lastFrameTime > FRAME_TIME) {
        currentFrame = (currentFrame + 1) % 4;
        lastFrameTime = SDL_GetTicks();
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 92, 148, 252, 255); // Sky blue
    SDL_RenderClear(renderer);

    // Ground
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &ground);

    // Platforms
    SDL_SetRenderDrawColor(renderer, 180, 100, 50, 255);
    for (int i = 0; i < numPlatforms; i++) {
        SDL_RenderFillRect(renderer, &platforms[i]);
    }

    // Coins
    for (int i = 0; i < numCoins; i++) {
        if (!coins[i].collected) {
            SDL_RenderCopy(renderer, coinTexture, NULL, &coins[i].rect);
        }
    }

    // Mario
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    if (currentState == RUN || currentState == JUMP) {
        SDL_RenderCopyEx(renderer, runMarioFrames[currentFrame], NULL, &playerRect, 0, NULL, flip);
    } else {
        SDL_RenderCopyEx(renderer, idleMario, NULL, &playerRect, 0, NULL, flip);
    }

    SDL_RenderPresent(renderer);
}

void cleanUp() {
    for (int i = 0; i < 4; ++i) {
        SDL_DestroyTexture(runMarioFrames[i]);
    }
    SDL_DestroyTexture(idleMario);
    SDL_DestroyTexture(coinTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

void handleCoinCollection(SDL_Rect* playerRect, Coin coins[], int numCoins) {
    for (int i = 0; i < numCoins; i++) {
        if (!coins[i].collected && checkCollision(*playerRect, coins[i].rect)) {
            coins[i].collected = true;
            printf("Coin collected!\n");  // Debug
        }
    }
}

int main(int argc, char* argv[]) {
    if (!init()) return -1;
    loadTextures();

    bool quit = false;
    SDL_Event e;
    bool moveLeft = false, moveRight = false;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_LEFT:
                        moveLeft = true;
                        facingRight = false;
                        break;
                    case SDLK_RIGHT:
                        moveRight = true;
                        facingRight = true;
                        break;
                    case SDLK_SPACE:
                        if (isOnGround) {
                            velocityY = -12;
                            currentState = JUMP;
                            jumping = true;
                        }
                        break;
                }
            } else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        moveLeft = false;
                        break;
                    case SDLK_RIGHT:
                        moveRight = false;
                        break;
                }
            }
        }

        if (moveLeft)
            playerRect.x -= 6;
        if (moveRight)
            playerRect.x += 6;

        if (moveLeft || moveRight)
            currentState = RUN;
        else if (!jumping)
            currentState = IDLE;

        handleCollisions(&playerRect, &velocityY, &isOnGround, &jumping, ground, platforms, numPlatforms);
        handleCoinCollection(&playerRect, coins, numCoins);
        updateFrame();
        render();
        SDL_Delay(16);
    }

    cleanUp();
    return 0;
}
