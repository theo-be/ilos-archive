/**
 * @file scene.c
 * @date 25/07/2022 Debut du projet
 * @brief Fichier regroupant les fonctions liees a la scene
*/


#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>


#include "scene.h"
#include "structures.h"
#include "variables.h"
#include "main.h"
#include "entities.h"
#include "miscellaneous.h"

/**
 * @fn void initScene (Scene *scene)
 * @param scene scene
 * @brief Initialise la scene
*/
void initScene (Scene *scene) {
    scene->camera.x = 0;
    scene->camera.y = 0;
    scene->map = NULL;
}

/**
 * @fn void doScene (Scene *scene, Player *player, int up, int down, int left, int right, unsigned int dt)
 * @param scene scene
 * @param player joueur
 * @param up haut
 * @param down bas
 * @param left gauche
 * @param right droite
 * @param dt delta
 * @brief Effectue toutes les actions liees a la scene
*/
void doScene (Scene *scene, Player *player, int up, int down, int left, int right, unsigned int dt) {
    moveCamera(scene, player, up, down, left, right, dt);
}

/**
 * @fn int **loadMap (const char *filename, int *mapH, int *mapW)
 * @param file_name nom du fichier
 * @param mapH hauteur de la carte
 * @param mapW largeur de la carte
 * @returns le tableau de la carte
 * @brief Charge la carte
*/
int **loadMap (const char *file_name, int *mapH, int *mapW) {

    FILE *file = NULL;
    int **m = NULL;

    file = fopen(file_name, "r");



    fscanf(file, "%d", mapH);
    fscanf(file, "%d", mapW);
    // printf("hauteur : %d\n", *mapH);
    // printf("longueur : %d\n", *mapW);



    m = malloc(sizeof(*m) * *mapH);
    if (m == NULL) {
        printf("Erreur creation tableau\n");
        SDL_Quit();
        exit(1);
    }
    for (int i = 0; i < *mapH; i++) {
        m[i] = malloc(sizeof(**m) * *mapW);
        if (m[i] == NULL) {
            printf("Erreur creation tableaux\n");
            SDL_Quit();
            exit(1);
        }
    }

    // Initialisation a 0
    for (int i = 0; i < *mapH; i++) {
        for (int j = 0; j < *mapW; j++) {
            m[i][j] = 0;
        }
    }



    for (int i = 0; i < *mapH; i++) {
        for (int j = 0; j < *mapW; j++) {
            fscanf(file, "%d", &m[i][j]);
        }
    }

    fclose(file);


    // for (int i = 0; i < MAP_HEIGHT; i++) {
    //     for (int j = 0; j < MAP_WIDTH; j++) {
    //         printf("%d, ", map[i][j]);
    //     }
    // }


    return m;

    // unloadMap(m);
}

/**
 * @fn SDL_Texture *loadTile (SDL_Renderer *renderer, char *tile_file)
 * @param renderer rendu
 * @param tile_file nom du fichier de la texture
 * @returns la texture
 * @brief Charge une texture 
*/
SDL_Texture *loadTile (SDL_Renderer *renderer, char *tile_file) {
    SDL_Surface *image;
    SDL_Texture *texture;

    image = SDL_LoadBMP(tile_file);
    if (image == NULL) {
        printf("Erreur de chargement de %s : %s\n", tile_file, SDL_GetError());
        return NULL;
    }

    texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);
    if (texture == NULL) {
        printf("Erreur de creation de texture : %s, %s\n", tile_file, SDL_GetError());
    }
    return texture;
}

/**
 * @fn SDL_Texture **loadTiles (SDL_Renderer *renderer)
 * @param renderer rendu
 * @returns le tableau de textures
 * @brief Charge les textures de la carte
*/
SDL_Texture **loadTiles (SDL_Renderer *renderer) {
    char tile_file[MAX_FILENAME_LENGTH];

    SDL_Texture **texture_arr = malloc(sizeof(*texture_arr) * TILES_TEXTURE_COUNT);

    if (texture_arr == NULL) {
        printf("Erreur d\'attribution memoire pour stocker les textures\n");
        return NULL;
    }


    for (int i = 1; i <= TILES_TEXTURE_COUNT; i++) {
        sprintf(tile_file, "ressources/fg%d.bmp", i);

        texture_arr[i - 1] = loadTile(renderer, tile_file);
    }

    return texture_arr;
}

/**
 * @fn void unloadTiles (SDL_Texture **texture_arr)
 * @param texture_aarr tableau de textures de la carte
 * @brief Decharge les textures de la carte
*/
void unloadTiles (SDL_Texture **texture_arr) {
    for (int i = 0; i < TILES_TEXTURE_COUNT; i++) {
        SDL_DestroyTexture(texture_arr[i]);
    }
    free(texture_arr);
    texture_arr = NULL;
}

/**
 * @fn void unloadMap (int **map, int mapH)
 * @param map carte a decharger
 * @param mapH hauteur de la carte
 * @brief Decharge la carte
*/
void unloadMap (int **map, int mapH) {
    for (int i = 0; i < mapH; i++) {
        // free(*(map + i));
        free(map[i]);
        map[i] = NULL;
    }
    free(map);
    map = NULL;
}

/**
 * @fn void showMap (SDL_Renderer *r, Scene *scene, SDL_Texture **texture_arr)
 * @param r rendu
 * @param scene scene
 * @param texture_arr tableau de textures de la carte
 * @brief Affiche la carte
*/
void showMap (SDL_Renderer *r, Scene *scene, SDL_Texture **texture_arr) {
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;


    int x1 = 0, x2 = 0, y1 = 0, y2 = 0, mx = 0, my = 0;


    // coordonees reelles de la zone de rendu

    // reste de la division, on veut commencer en dehors de l'ecran
    x1 = (scene->camera.x) % TILE_SIZE  * -1;
    x2 = x1 + MAP_RENDER_WIDTH * TILE_SIZE + (x1 == 0 ? 0 : TILE_SIZE);

    // reste de la division, on veut commencer en dehors de l'ecran
    y1 = (scene->camera.y) % TILE_SIZE  * -1;
    y2 = y1 + MAP_RENDER_HEIGHT * TILE_SIZE + (y1 == 0 ? 0 : TILE_SIZE);

    // quoi de la fuck ?
    mx = scene->camera.x / TILE_SIZE;
    my = scene->camera.y / TILE_SIZE;

    for (int y = y1; y < y2; y += TILE_SIZE) {
        for (int x = x1; x < x2; x += TILE_SIZE) {

            if (mx >= 0 && my >= 0 && mx < scene->map_width && my < scene->map_height) {
                int id = scene->map[my][mx];
                // switch (id) {
                //     case 0:
                //         SDL_SetRenderDrawColor(r, 255, 0, 0, 0);
                //         break;
                //     // case 1:
                //     //     SDL_SetRenderDrawColor(r, 0, 0, 255, SDL_ALPHA_OPAQUE);
                //     //     break;
                //     // case 2:
                //     //     SDL_SetRenderDrawColor(r, 255, 255, 0, SDL_ALPHA_OPAQUE);
                //     //     break;
                //     default:
                //         break;
                // }
                
                rect.y = y;
                rect.x = x;
                

                if (id > 0) {
                    SDL_RenderCopy(r, texture_arr[id - 1], NULL, &rect);
                } // else SDL_RenderDrawRect(r, &rect);

            }

            mx++;
        }
        mx = scene->camera.x / TILE_SIZE;

        my++;
    }
}

/**
 * @fn void moveCamera (Scene *scene, Player *player, int up, int down, int left, int right, unsigned int dt)
 * @param scene scene
 * @param player joueur
 * @param up haut
 * @param down bas
 * @param left gauche
 * @param right droite
 * @param dt delta
 * @brief Deplace la camera
*/
void moveCamera (Scene *scene, Player *player, int up, int down, int left, int right, unsigned int dt) {

    // float dx = 0.0;
    // if (left) {
    //     dx = -PLAYER_DEFAULT_MOVING_SPEED * (dt / 1000.0);
    //     scene->camera.x += (int)dx;
    // }
    // if (right) {
    //     dx = PLAYER_DEFAULT_MOVING_SPEED * (dt / 1000.0);
    //     scene->camera.x += (int)dx;
    // }
    // if (up) {
    //     dx = -PLAYER_DEFAULT_MOVING_SPEED * (dt / 1000.0);
    //     scene->camera.y += (int)dx;
    // }
    // if (down) {
    //     dx = PLAYER_DEFAULT_MOVING_SPEED * (dt / 1000.0);
    //     scene->camera.y += (int)dx;
    // }

    

    // if (player->hitbox.x + player->dx < MAP_RENDER_WIDTH * TILE_SIZE - MAX_AREA_WIDTH || player->hitbox.x + player->hitbox.w + player->dx > MAX_AREA_WIDTH) {
    //     scene->camera.x += 2 * player->dx;
    //     player->hitbox.x -= player->dx;
    // }

    // Centrer la camera sur le joueur

    // scene->camera.x = player->hitbox.x - MAP_RENDER_WIDTH * TILE_SIZE / 2;
    // scene->camera.y = player->hitbox.y - MAP_RENDER_HEIGHT * TILE_SIZE / 2;

    // Ajuster la camera selon la position du joueur

    SDL_Rect screen_position = {0, 0, TILE_SIZE, TILE_SIZE};

    screen_position.x = player->hitbox.x - scene->camera.x;
    screen_position.y = player->hitbox.y - scene->camera.y;

    // printf("AIRE : %d, %d\n", MAX_AREA_WIDTH, MAX_AREA_HEIGHT);
    // printf("POSITION : %d, %d\n", screen_position.x, screen_position.y);


    if (screen_position.x > MAX_AREA_WIDTH) {
        scene->camera.x = player->hitbox.x - MAX_AREA_WIDTH;
    } else if (screen_position.x < MAP_RENDER_WIDTH * TILE_SIZE - MAX_AREA_WIDTH) {
        scene->camera.x = player->hitbox.x - (MAP_RENDER_WIDTH * TILE_SIZE - MAX_AREA_WIDTH);
    }


    if (screen_position.y > MAX_AREA_HEIGHT) {
        scene->camera.y = player->hitbox.y - MAX_AREA_HEIGHT;
    } else if (screen_position.y < MAP_RENDER_HEIGHT * TILE_SIZE - MAX_AREA_HEIGHT) {
        scene->camera.y = player->hitbox.y - (MAP_RENDER_HEIGHT * TILE_SIZE - MAX_AREA_HEIGHT);
    }
    
    scene->camera.x = i_min(i_max(scene->camera.x, 0), (scene->map_width * TILE_SIZE) - (MAP_RENDER_WIDTH - 1) * TILE_SIZE);
    scene->camera.y = i_min(i_max(scene->camera.y, 0), (scene->map_height * TILE_SIZE) - (MAP_RENDER_HEIGHT - 1) * TILE_SIZE);
}

/**
 * @fn void centerCameraToPlayer (Scene* scene, Player *player)
 * @param scene scene
 * @param player joueur
 * @brief Centre la camera sur le joueur
*/
void centerCameraToPlayer (Scene* scene, Player *player) {
    // int next_camx = 0;
    // int next_camy = 0;
    // next_camx = player->hitbox.x + player->hitbox.w / 2 - (MAP_RENDER_WIDTH * TILE_SIZE / 2); 
    // next_camy = player->hitbox.y + player->hitbox.h / 2 - (MAP_RENDER_HEIGHT * TILE_SIZE / 2);


    // if (next_camx != scene->camera.x) {
    //     scene->camera.x = next_camx;
    //     player->hitbox.x = (MAP_RENDER_WIDTH * TILE_SIZE / 2) - player->hitbox.w / 2;
    // }
    // if (next_camy != scene->camera.y) {
    //     scene->camera.y = next_camy;
    //     player->hitbox.y = (MAP_RENDER_HEIGHT * TILE_SIZE / 2) - player->hitbox.h / 2;
    // }
    
    scene->camera.x = player->hitbox.x - MAP_RENDER_WIDTH * TILE_SIZE / 2;
    scene->camera.y = player->hitbox.y - MAP_RENDER_HEIGHT * TILE_SIZE / 2;


    scene->camera.x = i_min(i_max(scene->camera.x, 0), (scene->map_width * TILE_SIZE) - WINDOW_WIDTH);
    scene->camera.y = i_min(i_max(scene->camera.y, 0), (scene->map_height * TILE_SIZE) - WINDOW_HEIGHT);
}

/**
 * @fn int isInsideMap(Scene *scene, int x, int y)
 * @param scene scene
 * @param x position x
 * @param y position y
 * @returns 1 si l'entite est dans la carte, 0 sinon
 * @brief Indique si l'entite est dans la carte ou non
*/
int isInsideMap(Scene *scene, int x, int y) {
    int collisionIdArray[SOLID_TILES_COUNT] = {
        2, 3
    };
    for (int i = 0; i < SOLID_TILES_COUNT; i++) {
        if (scene->map[y][x] == collisionIdArray[i]) return 1;
    }
    return 0;
}

/**
 * @fn void playerInteraction (Player *player, EntityList *entity_list, char ***dialogues, MessageBox *message_box, int interaction)
 * @param player joueur
 * @param entity_list liste des entites
 * @param dialogues dialogues
 * @param message_box boite de message
 * @param interaction id de l'interaction a effectuer
 * @brief Effectue la bonne interaction declenchee avec la touche d'interaction
 * 1 : dialogue
 * 
*/
void playerInteraction (Player *player, EntityList *entity_list, char ***dialogues, MessageBox *message_box, int interaction) {
    switch (interaction) {
    case 1:
        // parler a l'entite
        talkToEntity(player, dialogues, checkEntityCollision(entity_list, player), message_box);
        break;
    
    default:
        break;
    }
}

