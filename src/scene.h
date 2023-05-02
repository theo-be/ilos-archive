
#ifndef SCENE_H
#define SCENE_H

#include <SDL.h>
#include "structures.h"


void initScene (Scene *scene);

void doScene (Scene *scene, Player *player, int up, int down, int left, int right, unsigned int dt);


SDL_Texture *loadTile (SDL_Renderer *renderer, char *tile_file);

SDL_Texture **loadTiles (SDL_Renderer *renderer);

void unloadTiles (SDL_Texture **texture_arr);



int **loadMap (const char *file_name, int *mapH, int *mapW);

void unloadMap (int **map, int mapH);

void showMap (SDL_Renderer *r, Scene *scene, SDL_Texture **texture_arr);

void moveCamera (Scene *scene, Player *player, int up, int down, int left, int right, unsigned int dt);

void centerCameraToPlayer (Scene* scene, Player *player);

int isInsideMap (Scene *scene, int x, int y);

void playerInteraction (Player *player, EntityList *entity_list, char ***dialogues, MessageBox *message_box, int interaction);

#endif