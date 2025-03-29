#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

/* Paramètres de la fenêtre : */
const int largeur = 800;
const int hauteur = 800;
const char * titre = "ESGI single shooter";

/* Coordonnées du joueur : */
int px, py;
/* Ligne de tir du joueur : */
int slpx, slpy;
/* Vitesse du joueur : */
int pv;

/* Coordonnées de l'adversaire : */
int ax, ay;
/* Vitesse de l'adversaire : */
int av;

void joueur_se_dirige_vers(int tx, int ty) {
  /* TODO : déplacer le joueur (px, py) vers la cible (tx, ty) */
  /* par pas de (pv)                                           */
}

void adversaire_se_dirige_vers(int tx, int ty) {
  /* TODO : déplacer l'adversaire (ax, ay) vers la cible       */
  /* (tx, ty) par pas de (av)                                  */
}

void joueur_regarde(int tx, int ty) {
  /* TODO : orienter la ligne de tir (slpx, slpy) vers         */
  /* (tx, ty)                                                  */
}

int distance_tir(int x, int y) {
  /* TODO : calculer la distance de la position (x, y) à la    */
  /* ligne de tir et remplacer 0x7fffffff par cette valeur     */
  return 0x7fffffff;
}

void place_adversaire() {
  /* TODO : placer l'adversaire (ax, ay) aléatoirement en      */
  /* bordure de la fenêtre (largeur, hauteur)                  */
}

int distance_joueur(int x, int y) {
  /* TODO : calculer la distance d'une position (x, y) au      */
  /* joueur (px, py)                                           */
  return 0x7fffffff;
}

void affichage(SDL_Surface * ecran) {
  /* Remplissage de l'écran par un gris foncé uniforme : */
  SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 51, 51, 51));
  /* Affichage du joueur : */
  filledCircleRGBA(ecran, px, py, 25, 51, 204, 51, 255);
  /* Affichage de l'adversaire : */
  filledCircleRGBA(ecran, ax, ay, 25, 204, 51, 51, 255);
  
  lineRGBA(ecran, px, py, slpx, slpy, 102, 255, 102, 255);
}

int main() {
  srand(time(NULL));
  /* Création d'une fenêtre SDL : */
  if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error in SDL_Init : %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  SDL_Surface * ecran = NULL;
  if((ecran = SDL_SetVideoMode(largeur, hauteur, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
    fprintf(stderr, "Error in SDL_SetVideoMode : %s\n", SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
  }
  SDL_WM_SetCaption(titre, NULL);
  
  /* Placement du joueur au centre de la fenêtre : */
  px = ecran->w / 2;
  py = ecran->h / 2;
  pv = 10;
  
  ax = ecran->w / 2;
  ay = 0;
  av = 5;
  
  int active = 1;
  int grab = 0;
  SDL_Event event;
  int last_mouse_x = px;
  int last_mouse_y = py;
  
  int score = 0;
  char display[100];
  
  while(active) {
    
    affichage(ecran);
    sprintf(display, "Score : %d", score);
    stringRGBA(ecran, 5, 5, display, 255, 255, 255, 255);
    SDL_Flip(ecran);
    
    while(SDL_PollEvent(&event)) {
      
      switch(event.type) {
        /* Utilisateur clique sur la croix de la fenêtre : */
        case SDL_QUIT : {
          active = 0;
        } break;
        
        /* Utilisateur enfonce une touche du clavier : */
        case SDL_KEYDOWN : {
          switch(event.key.keysym.sym) {
            /* Touche Echap : */
            case SDLK_ESCAPE : {
              active = 0;
            } break;
          }
        } break;
        
        /* Utilisateur commence le clic : */
        case SDL_MOUSEBUTTONDOWN : {
          switch(event.button.button) {
            case SDL_BUTTON_LEFT : {
              grab = 1;
              last_mouse_x = event.button.x;
              last_mouse_y = event.button.y;
            } break;
            
            case SDL_BUTTON_RIGHT : {
              if(abs(distance_tir(ax, ay)) < 25) {
                ++score;
                ++pv;
                ++av;
                place_adversaire();
              }
            } break;
          }
        } break;
        
        /* Utilisateur relâche le clic : */
        case SDL_MOUSEBUTTONUP : {
          switch(event.button.button) {
            case SDL_BUTTON_LEFT : {
              grab = 0;
            } break;
          }
        } break;
        
        /* Utilisateur bouge la souris : */
        case SDL_MOUSEMOTION : {
          last_mouse_x = event.motion.x;
          last_mouse_y = event.motion.y;
        } break;
      }
    }
    if(grab) {
      joueur_se_dirige_vers(last_mouse_x, last_mouse_y);
    }
    joueur_regarde(last_mouse_x, last_mouse_y);
    adversaire_se_dirige_vers(px, py);
    if(distance_joueur(ax, ay) < 25) {
      SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 204, 51, 51));
      filledCircleRGBA(ecran, px, py, 25, 0, 0, 0, 255);
    stringRGBA(ecran, 5, 5, display, 255, 255, 255, 255);
      SDL_Flip(ecran);
      SDL_Delay(1000);
      active = 0;
    }
    SDL_Delay(1000 / 60);
  }
  
  SDL_FreeSurface(ecran);
  SDL_Quit();
  exit(EXIT_SUCCESS);
}