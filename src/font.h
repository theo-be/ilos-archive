

#ifndef FONT_H
#define FONT_H

#include <SDL.h>

#include "structures.h"
#include "main.h"

SDL_Texture **loadFont (SDL_Renderer *renderer);

void unloadFont (SDL_Texture **texture_arr);

void displayFont (SDL_Renderer *renderer, SDL_Texture **texture_arr, const char *text, int x, int y, int font_size);

void displayMessageBox (SDL_Renderer *renderer, SDL_Texture **texture_arr, const char *text, int x, int y, int font_size);

void displayDialogBox (SDL_Renderer *renderer, SDL_Texture **texture_arr, const MessageBox *message_box, int x, int y);

char ***loadDialogues ();

void unloadDialogues (char ***dialogues);


#endif