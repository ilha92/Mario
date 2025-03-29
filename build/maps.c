#include <SDL/SDL.h>   // Inclusion de la bibliothèque SDL classique
#include <stdio.h>      // Pour afficher des messages avec printf
#include <stdbool.h>    // Pour utiliser les booléens (vrai/faux)

int main() {
    // 1. Initialisation de la SDL (ici, SDL_VIDEO pour la fenêtre)
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());  // Affiche un message si erreur
        return 1;  // Quitte le programme en cas d'échec
    }

    // 2. Création de la fenêtre SDL
    SDL_Surface *window = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!window) {
        printf("Erreur création fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_WM_SetCaption("Mon Premier Jeu SDL", NULL);  // Titre de la fenêtre

    // 3. Boucle principale du jeu
    bool gameRunning = true;  // Variable pour savoir si le jeu continue
    SDL_Event event;          // Variable pour stocker les événements (clavier, souris, etc.)

    while (gameRunning) {
        // 4. Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {  // Si on clique sur la croix pour fermer la fenêtre
                gameRunning = false;  // On arrête la boucle
            }
        }

        // 5. Dessin du fond de couleur (remplir avec une couleur bleue ciel)
        SDL_FillRect(window, NULL, SDL_MapRGB(window->format, 135, 206, 235));  // Remplit avec du bleu ciel
        SDL_Flip(window);  // Met à jour l'affichage
    }

    // 6. Sortie propre : nettoyage et fermeture
    SDL_Quit();
    return 0;
}
