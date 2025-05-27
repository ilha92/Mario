// main.c
#define SDL_MAIN_HANDLED
#define FRAME_TIME 100

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>

/* Partie include */
#include "collision.h"
#include "coin.h"
#include "score.h"
#include "enemy.h"
#include "config.h"
#include "player.h"
#include "powerup.h"
#include "texture.h"


/* Déclarations globales */
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* runMarioFrames[4];
SDL_Texture* idleMario = NULL;
SDL_Texture* coinTexture = NULL;
SDL_Texture* enemyTexture = NULL;
SDL_Texture* menuTexture = NULL;
SDL_Texture* pauseTexture = NULL;
SDL_Rect menuRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; // Taille de l'image du menu
SDL_Rect pauseRect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}; // Taille de l'image de la pause
typedef enum { IDLE, RUN, JUMP } State;
State currentState = IDLE;

SDL_Rect playerRect = {100, 100, 50, 50};
// Variables globales pour l'audio
Mix_Music* bgMusic = NULL;
Mix_Chunk* coinSound = NULL;
Mix_Chunk* jumpSound = NULL;
Mix_Chunk* pauseSound = NULL;
Mix_Chunk* gameOverSound = NULL; 

float velocityY = 0;
bool isOnGround = false;
bool jumping = false;
bool facingRight = true;
bool gameOver = false;
bool musicMuted = false;

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

PowerUp powerUps[10];
int numPowerUps = 0;
bool isInvincible = false;
bool isBig = false; // Indique si le joueur est sous l'effet du champignon
Uint32 bigStartTime = 0; // Temps de début de l'effet du champignon
Uint32 invincibilityStartTime = 0;

bool initAudio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur Mix_OpenAudio: %s\n", Mix_GetError());
        return false;
    }
    return true;
}

void loadSounds() {
    bgMusic = Mix_LoadMUS("Music/Mario.mp3");
    if (!bgMusic) {
        printf("Erreur chargement bgMusic: %s\n", Mix_GetError());
    }
    coinSound = Mix_LoadWAV("Audio/Super_Mario_Bros/smb_coin.wav");
    if (!coinSound) {
        printf("Erreur chargement coinSound: %s\n", Mix_GetError());
    }
    jumpSound = Mix_LoadWAV("Audio/Super_Mario_Bros/smb_jump-small.wav");
    if (!jumpSound) {
        printf("Erreur chargement jumpSound: %s\n", Mix_GetError());
    }
    pauseSound = Mix_LoadWAV("Audio/Super_Mario_Bros/smb_pause.wav");
    if (!pauseSound) {
        printf("Erreur chargement pauseSound: %s\n", Mix_GetError());
    }
    gameOverSound = Mix_LoadWAV("Audio/Super_Mario_Bros/smb_mariodie.wav");
    if (!gameOverSound) {
        printf("Erreur chargement gameOverSound: %s\n", Mix_GetError());
    }
}
void playBackgroundMusic() {
    if (bgMusic && !musicMuted) {
        Mix_PlayMusic(bgMusic, -1); // -1 pour boucle infinie
    }
}

void playCoinSound() {
    if (coinSound) Mix_PlayChannel(-1, coinSound, 0);
}

void playJumpSound() {
    if (jumpSound) Mix_PlayChannel(-1, jumpSound, 0);
}

void playPauseSound() {
    if (pauseSound) Mix_PlayChannel(-1, pauseSound, 0);
}
void playGameOverSound() {
    if (gameOverSound) Mix_PlayChannel(-1, gameOverSound, 0);
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
        runMarioFrames[i] = loadTexture(path, renderer); // Passez le renderer ici
    }
    idleMario = loadTexture("Images/idle_mario/0.png", renderer); // Passez le renderer ici
    coinTexture = loadTexture("Images/idle_coin/0.png", renderer); // Passez le renderer ici
    enemyTexture = loadTexture("Images/walk_goomba/0.png", renderer); // Passez le renderer ici
}

void loadMenuAndPauseTextures() {
    menuTexture = loadTexture("Images/UI_SDL_Mario_Bros.png", renderer); // Passez le renderer ici
    pauseTexture = loadTexture("Images/UI_Pause.png", renderer); // Passez le renderer ici
    if (!menuTexture || !pauseTexture) {
        printf("Erreur chargement des textures du menu ou de la pause.\n");
        exit(1);
    }
}
void toggleMusic() {
    if (musicMuted) {
        Mix_ResumeMusic(); // Réactive la musique
        musicMuted = false;
    } else {
        Mix_PauseMusic(); // Coupe la musique
        musicMuted = true;
    }
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

// Fonction pour afficher le texte à l'écran
void displayText(SDL_Renderer* renderer, const char* text, int x, int y, TTF_Font* font, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &destRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
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
    
// Rendu des power-ups
    for (int i = 0; i < numPowerUps; i++) {
        if (!powerUps[i].collected) {
            SDL_Rect powerUpRect = powerUps[i].rect;
            powerUpRect.x -= camera.x; // Ajuster la position en fonction de la caméra
            powerUpRect.y -= camera.y;

            if (powerUps[i].type == STAR) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Jaune pour l'étoile
            } else if (powerUps[i].type == MUSHROOM) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge pour le champignon
            }
            SDL_RenderFillRect(renderer, &powerUpRect);
        }
    }
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

    // Vérification des collisions avec les power-ups
    handlePowerUpCollection(&playerRect, powerUps, numPowerUps, &isInvincible, &invincibilityStartTime, &playerRect, &isBig, &bigStartTime);
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

void displayMenu(SDL_Renderer* renderer, SDL_Texture* menuTexture, TTF_Font* font) {
    bool inMenu = true;
    SDL_Event e;

    while (inMenu) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, menuTexture, NULL, &menuRect); // on utilise menuRect pour réduire la taille

        SDL_Color black = {0, 0, 0, 255}; // Noir
        SDL_Color white = {255, 255, 255, 255};

        // Définir les dimensions de l'encadrement
        SDL_Rect borderRect = {SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 200, 500, 250};
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Couleur bleue pour le remplissage
        SDL_RenderFillRect(renderer, &borderRect); // Remplir le cadre
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Couleur blanche pour la bordure
        SDL_RenderDrawRect(renderer, &borderRect); // Dessiner la bordure

        // Afficher les instructions
        displayText(renderer, "Touches à connaitre :", SCREEN_WIDTH / 2 - 240, SCREEN_HEIGHT / 2 - 180, font, black);
        displayText(renderer, "Fleche gauche : Deplacer Mario à gauche", SCREEN_WIDTH / 2 - 240, SCREEN_HEIGHT / 2 - 140, font, black);
        displayText(renderer, "Fleche droite : Deplacer Mario à droite", SCREEN_WIDTH / 2 - 240, SCREEN_HEIGHT / 2 - 110, font, black);
        displayText(renderer, "Espace : Sauter", SCREEN_WIDTH / 2 - 240, SCREEN_HEIGHT / 2 - 80, font, black);
        displayText(renderer, "P : Pause", SCREEN_WIDTH / 2 - 240, SCREEN_HEIGHT / 2 - 50, font, black);
        displayText(renderer, "M : Mute/Unmute la musique", SCREEN_WIDTH / 2 - 240, SCREEN_HEIGHT / 2 - 20, font, black);

        // Encadrement pour le texte principal
        SDL_Rect startRect = {SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT - 120, 500, 50};
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Couleur bleue pour le remplissage
        SDL_RenderFillRect(renderer, &startRect); // Remplir le cadre
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Couleur blanche pour la bordure
        SDL_RenderDrawRect(renderer, &startRect); // Dessiner la bordure
        // Afficher le texte principal
        displayText(renderer, "Appuyez sur ENTREE pour commencer", SCREEN_WIDTH / 2 - 240, SCREEN_HEIGHT - 100, font, white);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit(0); // Quitter le jeu
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                inMenu = false;
            }
        }
    }
}
void displayPause(SDL_Renderer* renderer, SDL_Texture* pauseTexture, TTF_Font* font) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseRect); // Utiliser pauseRect pour réduire la taille

    SDL_Color white = {255, 255, 255, 255};
    displayText(renderer, " ", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 25, font, white);

    SDL_RenderPresent(renderer);
}
/* Fonction principale */
int main(int argc, char* argv[]) {
    if (!init()) return -1;
    loadTextures();
    loadMenuAndPauseTextures();
    // Initialisation de l'audio    
    if (!initAudio()) return -1;
    loadSounds();
    playBackgroundMusic();
   TTF_Font* font = TTF_OpenFont("Images/arial.ttf", 24);
    if (!font) {
        printf("Erreur chargement police : %s\n", TTF_GetError());
        return -1;
    }
     // Afficher le menu principal
    displayMenu(renderer, menuTexture, font);

    Player player;
    initPlayer(&player);

    // Initialisation des ennemis
    initEnemies(enemies, &numEnemies, platforms, numPlatforms);
    initPowerUps(powerUps, &numPowerUps, platforms, numPlatforms);

    bool quit = false;
    bool paused = false; // Déclarez la variable paused ici
    SDL_Event e;
    bool moveLeft = false, moveRight = false;
    bool gameOver = false;

while (!quit) {
    // Vérification des vies du joueur
    checkPlayerLives(&player);
    if (!player.alive) {
        gameOver = true;
    }

    // Si gameOver est actif, afficher l'écran Game Over et attendre une entrée
    if (gameOver) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_Color white = {255, 255, 255, 255};
        
        playGameOverSound(); // Joue le son "Game Over"
        
        displayText(renderer, "Game Over", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, font, white);
        SDL_RenderPresent(renderer);
        
        // Boucle d'attente pour quitter
        bool waitingForInput = true;
        while (waitingForInput) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                    waitingForInput = false;
                    break;
                } else if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_ESCAPE) {
                        quit = true;
                        waitingForInput = false;
                        break;
                    }
                }
            }
        }
        break; // Quitter la boucle principale
    }
    
    // Gestion des événements
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            quit = true;
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE: quit = true; break;
                case SDLK_p: paused = !paused; playPauseSound(); // Joue le son de pause
                break;
                case SDLK_LEFT: moveLeft = true; facingRight = false; break;
                case SDLK_RIGHT: moveRight = true; facingRight = true; break;
                case SDLK_SPACE:
                    if (isOnGround) {
                        velocityY = -12;  // Saut
                        jumping = true;
                        currentState = JUMP;
                        playJumpSound(); // Joue le son de saut
                    }
                    break;
                case SDLK_m:
                    toggleMusic(); // Coupe ou réactive la musique
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
    
    // Mise à jour de la caméra et de l'état du joueur
    updateCamera(&camera, &playerRect);
    if (moveLeft || moveRight)
        currentState = RUN;
    else if (!jumping)
        currentState = IDLE;
    
    // Si le jeu est en pause, afficher l'écran de pause et passer au prochain cycle
    if (paused) {
        displayPause(renderer, pauseTexture, font);
        SDL_Delay(100);
        continue;
    }
    
    // Mises à jour diverses
    handleEnemyCollisions(&player, enemies, numEnemies, isInvincible);
    handleCollisions(&playerRect, &velocityY, &isOnGround, &jumping, ground, platforms, numPlatforms);
    handleCoinCollection(&playerRect, coins, numCoins, &score);
    update_player_and_camera(&player, &camera);
    renderPlayer(renderer, &player, camera);
    
    for (int i = 0; i < numCoins; i++) {
        if (coins[i].collected)
            spawnCoin(coins, numCoins, ground, platforms, numPlatforms);
    }
    
    moveEnemies(enemies, numEnemies, platforms, numPlatforms);
    updateEnemies(enemies, numEnemies, &playerRect, &player, &score, platforms, numPlatforms, &velocityY, isInvincible);    
    updateFrame();
    render(font);
    renderPowerUps(renderer, powerUps, numPowerUps);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
}
// Nettoyage de l'audio
void cleanupAudio() {
    if (coinSound) Mix_FreeChunk(coinSound);
    if (jumpSound) Mix_FreeChunk(jumpSound);
    if (pauseSound) Mix_FreeChunk(pauseSound);
    if (gameOverSound) Mix_FreeChunk(gameOverSound); // Libère le son "Game Over"
    if (bgMusic) Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();
}
// Nettoyage des ressources
SDL_DestroyTexture(menuTexture);
SDL_DestroyTexture(pauseTexture);
cleanUp();
cleanUpPowerUpTextures();
return 0;
}