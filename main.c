#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FRAME_TIME 100

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* runMarioFrames[4];
SDL_Texture* idleMarioFrame;
int currentFrame = 0;
Uint32 lastFrameTime = 0;

typedef enum { IDLE, RUN } State;
State currentState = IDLE;

SDL_Rect playerRect = {100, 100, 50, 50};
float velocityY = 0;
bool isOnGround = false;
bool jumping = false;

bool leftPressed = false;
bool rightPressed = false;
SDL_RendererFlip flip = SDL_FLIP_NONE;

SDL_Rect ground = {0, 550, 800, 50};         // Sol
SDL_Rect platform = {300, 400, 200, 20};     // Plateforme

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (!IMG_Init(IMG_INIT_PNG)) return false;

    window = SDL_CreateWindow("Mario Sprite & Collision",
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

void loadRunFrames() {
    runMarioFrames[0] = loadTexture("Images/run_mario/0.png");
    runMarioFrames[1] = loadTexture("Images/run_mario/1.png");
    runMarioFrames[2] = loadTexture("Images/run_mario/2.png");
    runMarioFrames[3] = loadTexture("Images/run_mario/3.png");

    idleMarioFrame = loadTexture("Images/idle_mario/0.png"); // Ajoute une image statique
}

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

void applyPhysics() {
    velocityY += 0.5f; // Gravité
    playerRect.y += (int)velocityY;
    isOnGround = false;

    if (checkCollision(playerRect, ground)) {
        playerRect.y = ground.y - playerRect.h;
        velocityY = 0;
        isOnGround = true;
        jumping = false;
    }

    if (checkCollision(playerRect, platform)) {
        if (velocityY >= 0 && playerRect.y + playerRect.h <= platform.y + 10) {
            playerRect.y = platform.y - playerRect.h;
            velocityY = 0;
            isOnGround = true;
            jumping = false;
        }
    }
}

void updateFrame() {
    if (SDL_GetTicks() - lastFrameTime > FRAME_TIME) {
        currentFrame = (currentFrame + 1) % 4;
        lastFrameTime = SDL_GetTicks();
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 92, 148, 252, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &ground);

    SDL_SetRenderDrawColor(renderer, 80, 40, 40, 255);
    SDL_RenderFillRect(renderer, &platform);

    if (currentState == RUN)
        SDL_RenderCopyEx(renderer, runMarioFrames[currentFrame], NULL, &playerRect, 0, NULL, flip);
    else
        SDL_RenderCopyEx(renderer, idleMarioFrame, NULL, &playerRect, 0, NULL, flip);

    SDL_RenderPresent(renderer);
}

void cleanUp() {
    for (int i = 0; i < 4; ++i) {
        SDL_DestroyTexture(runMarioFrames[i]);
    }
    SDL_DestroyTexture(idleMarioFrame);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!init()) return -1;
    loadRunFrames();

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_LEFT:
                        leftPressed = true;
                        flip = SDL_FLIP_HORIZONTAL;
                        break;
                    case SDLK_RIGHT:
                        rightPressed = true;
                        flip = SDL_FLIP_NONE;
                        break;
                    case SDLK_SPACE:
                        if (isOnGround) {
                            velocityY = -15; // saut plus haut
                            jumping = true;
                        }
                        break;
                }
            }

            if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        leftPressed = false;
                        break;
                    case SDLK_RIGHT:
                        rightPressed = false;
                        break;
                }
            }
        }

        if (leftPressed) {
            playerRect.x -= 5;
            if (isOnGround) currentState = RUN;
        }

        if (rightPressed) {
            playerRect.x += 5;
            if (isOnGround) currentState = RUN;
        }

        if (!leftPressed && !rightPressed && isOnGround) {
            currentState = IDLE;
        }

        if (jumping && (leftPressed || rightPressed)) {
            currentState = RUN;
        }

        applyPhysics();
        updateFrame();
        render();
        SDL_Delay(16);
    }

    cleanUp();
    return 0;
}
