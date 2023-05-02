
#ifndef INERFACE_H
#define INTERFACE_H

#include <SDL.h>


#include "font.h"



void showInterface (SDL_Renderer *renderer, SDL_Texture **texture_arr, Scene *scene, Player *player, MessageBox *message_box);



#endif