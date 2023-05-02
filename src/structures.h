
#ifndef STRUCTURES_H 
#define STRUCTURES_H

#include <SDL.h>
#include "variables.h"

/**
 * @struct Player
 * @brief Structure du joueur
*/
struct player {
    SDL_Rect hitbox;
    int p0x;
    int p0y;
    float dx;
    float d0x;
    float dy;
    float d0y;
    float ax;
    float ay;
    int hp;
    int touch_ground;
    int jumping;
    int moving_speed;
    float flight_time;
    float invincibility_time;
    int playable;
    int available_interaction;
    SDL_Texture *texture;
};
typedef struct player Player;



/**
 * @struct Entity
 * @brief Structure des entites
*/
struct entity {
    unsigned int id;
    SDL_Rect hitbox;
    int hp;
    int passive;
    int textureId;
    char *name;
    int touch_ground;
    int flight_time;
    int p0x;
    int p0y;
    float dx;
    float d0x;
    float dy;
    float d0y;
    float ax;
    float ay;
    int dialogue_id;
};
typedef struct entity Entity;

/**
 * @struct EntityList
 * @brief Structure de la liste chainee des entites
*/
struct entitylist {
    Entity entity;
    struct entitylist *next;
};
typedef struct entitylist EntityList;

/**
 * @struct Scene
 * @brief Structure de la scene
*/
struct scene {
    int **map;
    int map_width;
    int map_height;
    SDL_Point camera;
};
typedef struct scene Scene;

/**
 * @struct Interface
 * @brief Structure de l'interface
*/
struct interface {
    SDL_Rect dimension;
};
typedef struct interface Interface;

/**
 * @struct MessageBox
 * @brief Structure de la boite de message
*/
struct messageBox {
    char *message;
    char *speaker;
    int used;
    // Utilise pour les dialogues
    int message_index;
    int font_size;
};
typedef struct messageBox MessageBox;


#endif