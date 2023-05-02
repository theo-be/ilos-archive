/**
 * @file interface.c
 * @date 
 * @brief Fichier regroupant les fonctions liees a l'interface
*/

#include <SDL.h>
#include <stdio.h>
#include <string.h>

#include "interface.h"
#include "structures.h"
#include "variables.h"
#include "font.h"
#include "miscellaneous.h"

/**
 * @fn void showInterface (SDL_Renderer *renderer, SDL_Texture **texture_arr, MessageBox *message_box)
 * @param renderer rendu
 * @param font_textures textures de la police
 * @param scene scene
 * @param player joueur
 * @param message_box boite de message
 * @brief Affiche l'interface
*/
void showInterface (SDL_Renderer *renderer, SDL_Texture **font_textures, Scene *scene, Player *player, MessageBox *message_box) {
    

    // Dialogues
    if (message_box->used) {
        // int font_size = 32;
        int win_center = WINDOW_WIDTH / 2 - (i_max(strlen(message_box->message) * message_box->font_size, strlen(message_box->speaker) * message_box->font_size)) / 2;
        displayDialogBox(renderer, font_textures, message_box, win_center, WINDOW_HEIGHT - DIALOGUE_OFFSET * 3 - 2 * message_box->font_size);
    }

    // Interaction 
    if (player->available_interaction == 1) {
        displayFont(renderer, font_textures, "E", player->hitbox.x - scene->camera.x, player->hitbox.y - player->hitbox.h - 10 - scene->camera.y, 32);
    }

    // Nombre de points de vie
    
    char hp[6] = {'\n'};
    sprintf(hp, "%d PV", player->hp);
    displayFont(renderer, font_textures, hp, 20, WINDOW_HEIGHT - 50, FONT_SIZE_INTERFACE);
}





