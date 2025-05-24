// main.c
#define SDL_MAIN_HANDLED
#define FRAME_TIME 100

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

/* Partie include */
#include "collision.h"
#include "coin.h"
#include "score.h"
#include "enemy.h"
#include "config.h"
#include "player.h"

/* Déclarations globales */
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* runMarioFrames[4];
SDL_Texture* idleMario = NULL;
SDL_Texture* coinTexture = NULL;
SDL_Texture* enemyTexture = NULL;

typedef enum { IDLE, RUN, JUMP } State;
State currentState = IDLE;

SDL_Rect playerRect = {100, 100, 50, 50};
float velocityY = 0;
bool isOnGround = false;
bool jumping = false;
bool facingRight = true;

SDL_Rect ground = {0, 550, 1600, 100}; // Taille augmentée

SDL_Rect platforms[7] = {
    {200, 450, 150, 20},
    {400, 380, 150, 20},
    {600, 280, 250, 20},
    {800, 380, 150, 20},
    {1000, 450, 150, 20},
    {1200, 380, 150, 20},
    {1400, 280, 250, 20}
};

int numPlatforms = 7;

Coin coins[5] = {
    {{220, 430, 30, 30}, false},
    {{420, 360, 30, 30}, false},
    {{550, 430, 30, 30}, false},
    {{200, 500, 30, 30}, false},
    {{380, 330, 30, 30}, false}
};

int numCoins = 5;

Enemy enemies[MAX_ENEMIES];
int numEnemies = 0;

int currentFrame = 0;
Uint32 lastFrameTime = 0;
int score = 0;

SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

/* Fonctions */
SDL_Texture* loadTexture(const char* path) {
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

    window = SDL_CreateWindow("Super Mario Bross", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
    enemyTexture = loadTexture("Images/walk_goomba/0.png");
}

void updateFrame() {
    if ((currentState == RUN || currentState == JUMP) && SDL_GetTicks() - lastFrameTime > FRAME_TIME) {
        currentFrame = (currentFrame + 1) % 4;
        lastFrameTime = SDL_GetTicks();
    }
}

void updateCamera(SDL_Rect* camera, SDL_Rect* playerRect) {
    camera->x = playerRect->x + playerRect->w / 2 - SCREEN_WIDTH / 2;
    if (camera->x < 0) camera->x = 0;
    if (camera->x > MAP_WIDTH - SCREEN_WIDTH) camera->x = MAP_WIDTH - SCREEN_WIDTH;

    camera->y = playerRect->y + playerRect->h / 2 - SCREEN_HEIGHT / 2;
    if (camera->y < 0) camera->y = 0;
    if (camera->y > MAP_HEIGHT - SCREEN_HEIGHT) camera->y = MAP_HEIGHT - SCREEN_HEIGHT;
}

void render(TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 92, 148, 252, 255);
    SDL_RenderClear(renderer);

    // Ground
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_Rect groundRender = ground;
    groundRender.x -= camera.x;
    groundRender.y -= camera.y;
    SDL_RenderFillRect(renderer, &groundRender);
    

    // Platforms
    SDL_SetRenderDrawColor(renderer, 180, 100, 50, 255);
    for (int i = 0; i < numPlatforms; i++) {
        SDL_Rect plat = platforms[i];
        plat.x -= camera.x;
        plat.y -= camera.y;
        SDL_RenderFillRect(renderer, &plat);
    }

    // Coins
    for (int i = 0; i < numCoins; i++) {
        if (!coins[i].collected) {
            SDL_Rect coinRect = coins[i].rect;
            coinRect.x -= camera.x;
            coinRect.y -= camera.y;
            SDL_RenderCopy(renderer, coinTexture, NULL, &coinRect);
        }
    }

    // Enemies
    for (int i = 0; i < numEnemies; i++) {
        if (enemies[i].alive) {
            SDL_Rect enemyRect = enemies[i].rect;
            enemyRect.x -= camera.x;
            enemyRect.y -= camera.y;
            SDL_RenderCopy(renderer, enemyTexture, NULL, &enemyRect);
        }
    }

    // Mario
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_Texture* marioTexture = (currentState == RUN || currentState == JUMP) ? runMarioFrames[currentFrame] : idleMario;
    SDL_Rect marioRender = playerRect;
    marioRender.x -= camera.x;
    marioRender.y -= camera.y;
    SDL_RenderCopyEx(renderer, marioTexture, NULL, &marioRender, 0, NULL, flip);

    // Score
    displayScore(renderer, font, score);

    SDL_RenderPresent(renderer);
}

void cleanUp() {
    for (int i = 0; i < 4; ++i) {
        if (runMarioFrames[i]) SDL_DestroyTexture(runMarioFrames[i]);
    }
    if (idleMario) SDL_DestroyTexture(idleMario);
    if (coinTexture) SDL_DestroyTexture(coinTexture);
    if (enemyTexture) SDL_DestroyTexture(enemyTexture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

 int playerLives = 3;

/* Fonction principale */
int main(int argc, char* argv[]) {
    if (!init()) return -1;
    loadTextures();

    TTF_Font* font = TTF_OpenFont("Images/arial.ttf", 24);
    if (!font) {
        printf("Erreur chargement police : %s\n", TTF_GetError());
        return -1;
    }
    Player player;
    initPlayer(&player);

    // Initialisation des ennemis
    initEnemies(enemies, &numEnemies, platforms, numPlatforms);

    bool quit = false;
    SDL_Event e;
    bool moveLeft = false, moveRight = false;

    while (!quit) {
        // Mise à jour du temps actuel
        Uint32 currentTime = SDL_GetTicks();
    
        // Réinitialisation des ennemis morts après 10 secondes
        for (int i = 0; i < numEnemies; i++) {
            if (!enemies[i].alive && SDL_GetTicks() - enemies[i].deathTime >= 10000) { // 10 secondes
                enemies[i].alive = 1; // Réactiver l'ennemi
                enemies[i].rect.x = enemies[i].initialX; // Réinitialiser à la position initiale
                int platformIndex = enemies[i].platformIndex;
                SDL_Rect p = platforms[platformIndex];
                enemies[i].rect.y = p.y - enemies[i].rect.h; // Réinitialiser sur la plateforme
                enemies[i].movingRight = 1; // Réinitialiser la direction
                printf("Ennemi %d respawn à la position initiale %d.\n", i, enemies[i].initialX);
            }
        }
    
        // Gestion des événements
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
    
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: quit = true; break;
                    case SDLK_LEFT: moveLeft = true; facingRight = false; break;
                    case SDLK_RIGHT: moveRight = true; facingRight = true; break;
                    case SDLK_SPACE:
                        if (isOnGround) {
                            velocityY = -12;  // Saut
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
    
        // Déplacement du joueur
        if (moveLeft) playerRect.x -= 6;
        if (moveRight) playerRect.x += 6;
    
        // Application de la gravité
        velocityY += 0.5f;
        playerRect.y += (int)velocityY;
    
        // Limites de la carte
        if (playerRect.x < 0) playerRect.x = 0;
        if (playerRect.x > MAP_WIDTH - playerRect.w) playerRect.x = MAP_WIDTH - playerRect.w;
        if (playerRect.y > MAP_HEIGHT - playerRect.h) {
            playerRect.y = MAP_HEIGHT - playerRect.h;
            isOnGround = true;
            velocityY = 0;
            jumping = false;
        }
    
        // Mise à jour de la caméra
        updateCamera(&camera, &playerRect);
    
        // Mise à jour de l'état du joueur
        if (moveLeft || moveRight) {
            currentState = RUN;
        } else if (!jumping) {
            currentState = IDLE;
        }
    
        // Gestion des collisions
        handleCollisions(&playerRect, &velocityY, &isOnGround, &jumping, ground, platforms, numPlatforms);

        // Vérification des collisions avec les pièces
        handleCoinCollection(&playerRect, coins, numCoins, &score);
        // Mettez à jour le joueur et la caméra
        update_player_and_camera(&player, &camera);

        // Vérifier les collisions avec les ennemis 
        handleEnemyCollisions(&player, enemies, numEnemies);

        // Vérifier les vies du joueur
        checkPlayerLives(&player);

        // Rendu du joueur
        renderPlayer(renderer, &player, camera);
    
        for (int i = 0; i < numCoins; i++) {
            if (coins[i].collected) {
                spawnCoin(coins, numCoins, ground, platforms, numPlatforms);
            }
        }
    
        // Déplacement des ennemis
        moveEnemies(enemies, numEnemies, platforms, numPlatforms);
        // Variable pour compter les morts du joueur
        static int deathCount = 0;

    // Collision avec les ennemis
            for (int i = 0; i < numEnemies; i++) {
                if (enemies[i].alive && SDL_HasIntersection(&playerRect, &enemies[i].rect)) {
                    if (playerRect.y + playerRect.h - 10 < enemies[i].rect.y) {
                        // Le joueur saute sur l'ennemi → tue l'ennemi
                        enemies[i].alive = 0;
                        enemies[i].deathTime = SDL_GetTicks(); // Enregistre le moment de la mort
                        velocityY = -10;  // Rebond
                        score += 100;     // Bonus de score
                        printf("Ennemi %d tué !\n", i);
                    } else {
                        // Le joueur touche l'ennemi de côté → il perd une vie
                        if (playerLives > 0) {
                            playerLives--;       // Perdre une vie
                            printf("Il vous reste %d vies.\n", playerLives);

                            if (playerLives <= 0) {
                                printf("Perdu ! Vous avez épuisé toutes vos vies.\n");
                                quit = true;  // Quitter le jeu après avoir perdu toutes les vies
                            }
                        }

                        // Désactiver temporairement l'ennemi et enregistrer le temps de mort
                        enemies[i].alive = 0;
                        enemies[i].deathTime = SDL_GetTicks(); // Enregistre le moment de la mort
                        printf("Ennemi %d désactivé après collision latérale.\n", i);
                    }
                }
            }

    updateFrame();
    render(font);
    SDL_Delay(16);

    }

    cleanUp();
    return 0;
}