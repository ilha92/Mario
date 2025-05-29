# Mario

# la commande pour executer le jeu

$ gcc -o mario main.c src/collision.c src/coin.c src/score.c src/enemy.c src/player.c src/powerup.c src/texture.c -I./include -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm -mconsole

# La commande pour lancer le jeu

./mario
