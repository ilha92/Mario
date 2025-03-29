#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

// Dimensions de la fenêtre
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Structure pour le personnage
SDL_Rect playerRect;
SDL_Texture* playerTexture;
SDL_Renderer* renderer;
SDL_Window* window;

// Variables pour le saut
int isJumping = 0;      // Savoir si le personnage est en train de sauter
float jumpSpeed = -15.0f; // Vitesse du saut (plus négatif = saut plus haut)
float gravity = 0.5f;     // La gravité qui attire le joueur vers le bas
float dy = 0;             // Vitesse verticale du personnage
int groundLevel = SCREEN_HEIGHT - 100; // Niveau du sol, où le personnage doit atterrir

// Vitesse de déplacement du personnage
int playerSpeed = 5;

// Fonction pour gérer le mouvement et les collisions
void handleMovement() {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_LEFT]) {
        if (playerRect.x > 0) {  // Limite gauche
            playerRect.x -= playerSpeed;
        }
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        if (playerRect.x + playerRect.w < SCREEN_WIDTH) {  // Limite droite
            playerRect.x += playerSpeed;
        }
    }
}

// Fonction de gestion du saut et de la gravité
void handleJump() {
    // Si l'utilisateur appuie sur la barre d'espace et n'est pas déjà en train de sauter
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_SPACE] && !isJumping) {
        dy = jumpSpeed;  // Applique la vitesse du saut
        isJumping = 1;   // Le personnage est maintenant en l'air
    }

    // Applique la gravité
    if (isJumping) {
        dy += gravity; // La gravité s'ajoute à la vitesse verticale
    }

    // Déplace le personnage verticalement
    playerRect.y += (int)dy;

    // Si le personnage touche le sol
    if (playerRect.y >= groundLevel) {
        playerRect.y = groundLevel; // Positionne le personnage au sol
        isJumping = 0;  // Le personnage n'est plus en l'air
        dy = 0; // Réinitialise la vitesse verticale
    }
}

// Fonction d'initialisation
int init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Mario Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return -1;
    }

    // Chargement de l'image du joueur
    SDL_Surface* playerSurface = IMG_Load("images/mario.png");
    if (!playerSurface) {
        printf("IMG_Load Error: %s\n", IMG_GetError());
        return -1;
    }

    playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_FreeSurface(playerSurface);
    if (!playerTexture) {
        printf("CreateTexture Error: %s\n", SDL_GetError());
        return -1;
    }

    // Initialisation du personnage (position de départ)
    playerRect.x = 100;
    playerRect.y = groundLevel;
    playerRect.w = 50;
    playerRect.h = 50;

    return 0;
}

// Fonction de nettoyage
void closeGame() {
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (init() < 0) {
        return -1;
    }

    int running = 1;
    SDL_Event e;

    // Boucle principale
    while (running) {
        // Gestion des événements
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Gestion du mouvement du personnage
        handleMovement();

        // Gestion du saut et de la gravité
        handleJump();

        // Effacer l'écran avec un fond (par exemple, bleu ou une texture de fond)
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Bleu pour le fond
        SDL_RenderClear(renderer);

        // Dessiner l'image de fond
        SDL_Surface* backgroundSurface = IMG_Load("images/BackgroundMB.png");
        SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
        SDL_FreeSurface(backgroundSurface);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        SDL_DestroyTexture(backgroundTexture);

        // Dessiner le personnage
        SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect);

        // Mettre à jour l'écran
        SDL_RenderPresent(renderer);

        // Délais pour la gestion du taux de rafraîchissement
        SDL_Delay(16); // 60 FPS environ
    }

    // Fermer le jeu
    closeGame();

    return 0;
}
