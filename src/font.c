/**
 * @file font.c
 * @date 
 * @brief Fichier regroupant les fonctions liees au texte et aux dialogues
*/


#include <SDL.h>
#include <stdio.h>
#include <string.h>

#include "font.h"
#include "variables.h"
#include "structures.h"
#include "miscellaneous.h"

/**
 * @fn SDL_Texture **loadFont (SDL_Renderer *renderer)
 * @param renderer rendu
 * @returns le tableau de texture
 * @brief Charge les textures de la police
*/
SDL_Texture **loadFont (SDL_Renderer *renderer) {
    SDL_Surface *image;
    SDL_Texture *texture;
    SDL_Texture **texture_arr = malloc(sizeof(*texture_arr) * (FONT_TEXTURE_COUNT + 10));
    if (texture_arr == NULL) {
        printf("Erreur allocation memoire lettres\n");
        return NULL;
    }

    char file_name[MAX_FILENAME_LENGTH];
    char letter = 'A';


    // MAJUSCULES
    for (int i = 0; i < FONT_LETTER_COUNT; i++) {
        sprintf(file_name, "ressources/letter_%c.bmp", letter + i);

        image = SDL_LoadBMP(file_name);
        if (image == NULL) {
            printf("Erreur de chargement de %s : %s\n", file_name, SDL_GetError());
            return NULL;
        }

        texture = SDL_CreateTextureFromSurface(renderer, image);
        SDL_FreeSurface(image);
        if (texture == NULL) {
            printf("Erreur de creation de texture : %s, %s", file_name, SDL_GetError());
        }

        texture_arr[i] = texture;
    }

    // CHIFFRES
    int number = 0;
    for (int j = FONT_LETTER_COUNT; j < FONT_LETTER_COUNT + FONT_NUMBER_COUNT; j++) {
        sprintf(file_name, "ressources/letter_%d.bmp", number);
        // printf("%s\n", file_name);
        number++;

        image = SDL_LoadBMP(file_name);
        if (image == NULL) {
            printf("Erreur de chargement de %s : %s\n", file_name, SDL_GetError());
            return NULL;
        }

        texture = SDL_CreateTextureFromSurface(renderer, image);
        SDL_FreeSurface(image);
        if (texture == NULL) {
            printf("Erreur de creation de texture : %s, %s", file_name, SDL_GetError());
        }

        texture_arr[j] = texture;
    }


    return texture_arr;
}

/**
 * @fn void unloadFont (SDL_Texture **texture_arr)
 * @param texture_arr tableau de textures de la police
 * @brief Decharge les textures de la police
*/
void unloadFont (SDL_Texture **texture_arr) {

    for (int i = 0; i < FONT_TEXTURE_COUNT; i++) {
        SDL_DestroyTexture(texture_arr[i]);
    }
    free(texture_arr);
    texture_arr = NULL;

}

/**
 * @fn void displayFont (SDL_Renderer *renderer, SDL_Texture **texture_arr, const char *text, int x, int y, int font_size)
 * @param renderer rendu
 * @param texture_arr tableau de textures de la police
 * @param text texte a afficher MAJ et CHIFFRES uniquement
 * @param x position x
 * @param y position y
 * @param font_size taille de la police
 * @brief Affiche un message a l'ecran
*/
void displayFont (SDL_Renderer *renderer, SDL_Texture **texture_arr, const char *text, int x, int y, int font_size) {
    SDL_Rect dimension = {x, y, font_size, font_size};

    size_t len = strlen(text);
    for (int i = 0; i < len; i++) {

        char c = text[i];

        dimension.x = i * dimension.w + x;

        // MAJUSCULES, CHIFFERS ET ESPACES UNIQUEMENT
        if (c != ' ') {
            // Chiffres
            if (c >= 48 && c <= 57) SDL_RenderCopy(renderer, texture_arr[c - 48 + FONT_LETTER_COUNT], NULL, &dimension);
            // majuscules
            else if (c >= 65 && c <= 90) SDL_RenderCopy(renderer, texture_arr[c - 65], NULL, &dimension);
            // minuscules
            // 
        }
        
    }
}

/**
 * @fn void displayMessageBox (SDL_Renderer *renderer, SDL_Texture **texture_arr, const char *text, int x, int y, int font_size)
 * @param renderer rendu
 * @param texture_arr tableau de textures de la police
 * @param text texte a afficher
 * @param x position x
 * @param y position y
 * @param font_size taille de la police
 * @brief Affiche une boite de message
*/
void displayMessageBox (SDL_Renderer *renderer, SDL_Texture **texture_arr, const char *text, int x, int y, int font_size) {
    int offset = DIALOGUE_OFFSET;
    SDL_Rect box_len = {x, y, strlen(text) * font_size + offset * 2, font_size + offset * 2};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &box_len);
    displayFont(renderer, texture_arr, text, x + offset, y + offset, font_size);
}

/**
 * @fn void displayDialogBox (SDL_Renderer *renderer, SDL_Texture **texture_arr, const MessageBox *message_box, int x, int y)
 * @param renderer rendu
 * @param texture_arr tableau de textures de la police
 * @param message_box boite de message
 * @param x position x
 * @param y position y
 * @brief Affiche une boite de dialogue
*/
void displayDialogBox (SDL_Renderer *renderer, SDL_Texture **texture_arr, const MessageBox *message_box, int x, int y) {
    // printf("%s a dit : %s\n", message_box->speaker, message_box->message);
    // displayFont(renderer, texture_arr, message_box->message, 0, 0, 50);
    int offset = DIALOGUE_OFFSET;
    // rectangle qui va couvrir le dialogue
    SDL_Rect r = {x, y, message_box->font_size * i_max(strlen(message_box->message), strlen(message_box->speaker)) + offset * 2, 2 * message_box->font_size + offset * 3};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &r);
    displayFont(renderer, texture_arr, message_box->speaker, x + offset, y + offset, message_box->font_size);
    displayFont(renderer, texture_arr, message_box->message, x + offset, y + offset * 2 + message_box->font_size, message_box->font_size);
}

/**
 * @fn char ***loadDialogues ()
 * @returns le tableau de dialogues
 * @brief Charge les dialogues
*/
char ***loadDialogues () {
    char ***d = NULL;

    FILE *file = fopen("data/dialogues", "r");
    if (file == NULL) {
        printf("Erreur chargement \'ressources/dialogues\'\n");
        fclose(file);
        return NULL;
    }

    int dialogue_count = -1;
    int message_count = -1;
    int letter_count = -1;


    // lecture du nombre de dialogues
    fscanf(file, "%d\n", &dialogue_count);
    printf("%d dialogues a charger\n", dialogue_count);
    if (dialogue_count == -1) {
        printf("Erreur lecture nombre de dialogues\n");
        fclose(file);
        return NULL;
    }

    d = malloc(dialogue_count * sizeof(*d));
    if (d == NULL) {
        printf("Erreur malloc premiere allocation\n");
        fclose(file);
        return NULL;
    }


    for (int i = 0; i < dialogue_count; i++) {

        // lecture du nombre de message par dialogue
        fscanf(file, "%d\n", &message_count);
        // printf("%d m, %d d\n", message_count, i);
        d[i] = malloc(message_count * sizeof(**d));
        if (d[i] == NULL) {
            printf("Erreur malloc deuxieme allocation\n");
            fclose(file);
            return NULL;
        }





        for (int j = 0; j < message_count; j++) {


            // lecture du nombre de caracteres par message
            fscanf(file, "%d\n", &letter_count);
            // printf("%d l, %d m, %d d\n", letter_count, j, i);
            d[i][j] = malloc(letter_count * sizeof(***d));
            if (d[i][j] == NULL) {
                printf("Erreur malloc trosieme allocation\n");
                fclose(file);
                return NULL;
            }

            // chargement des lignes de dialogues

            fgets(d[i][j], letter_count, file);
            // printf("%s\n", d[i][j]);
            // SDL_Delay(500);

        }
    }

    



    fclose(file);

    // return NULL;
    return d;
}

/**
 * @fn void unloadDialogues (char ***dialogues)
 * @param dialogues dialogues a decharger
 * @brief Decharge les dialogues
*/
void unloadDialogues (char ***dialogues) {

    FILE *f = fopen("data/dialogues", "r");

    int n, m;
    fscanf(f, "%d\n", &n);
    fscanf(f, "%d\n", &m);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            free(dialogues[i][j]);
            dialogues[i][j] = NULL;
        }
        free(dialogues[i]);
        dialogues[i] = NULL;
        
        do {

        } while (fgetc(f) != '<');
        fscanf(f, "%d\n", &m);
    }

    free(dialogues);
    dialogues = NULL;
}


