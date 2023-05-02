/**
 * @file entities.c
 * @date 25/07/2022 Debut du projet
 * @brief Fichier regroupant les fonctions liees aux entites
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "entities.h"
#include "scene.h"
#include "variables.h"
#include "structures.h"
#include "miscellaneous.h"
#include "font.h"


/* -------------------------------------------------- */
// Joueur
/* -------------------------------------------------- */


/**
 * @fn void initPlayer (SDL_Renderer *r, Player *p)
 * @param r rendu
 * @param p joueur
 * @brief Initialise le joueur
 * @date 25/08/2022
*/
void initPlayer (SDL_Renderer *r, Player *p) {
    p->hitbox.w = TILE_SIZE;
    p->hitbox.h = TILE_SIZE;
    // p->hitbox.x = WINDOW_WIDTH / 2 - p->hitbox.w;
    // p->hitbox.y = WINDOW_HEIGHT / 2 - p->hitbox.h;
    p->hitbox.x = 5 * TILE_SIZE;
    p->hitbox.y = 26 * TILE_SIZE;
    p->p0x = 0;
    p->p0y = 0;
    p->dx = 0;
    p->d0x = 0;
    p->dy = 0;
    p->d0y = 100;
    p->ax = 0;
    p->ay = 1000;
    p->moving_speed = PLAYER_DEFAULT_MOVING_SPEED;
    p->touch_ground = 1;
    p->jumping = 0;
    p->flight_time = 0.0;
    p->hp = 10;
    p->invincibility_time = 0.0;
    p->playable = 1;
    p->available_interaction = 0;
    p->texture = loadPlayerTexture(r);
}

/**
 * @fn SDL_Texture *loadPlayerTexture (SDL_Renderer *renderer)
 * @param renderer rendu
 * @returns le pointeur vers une texture
 * @brief Charge la texture du joueur
*/
SDL_Texture *loadPlayerTexture (SDL_Renderer *renderer) {
    SDL_Surface *image;
    SDL_Texture *texture;

    image = SDL_LoadBMP("ressources/player1.bmp");
    if (image == NULL) {
        printf("Erreur chargement d\'image du joueur : %s\n", SDL_GetError());
    }
    texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);
    if (texture == NULL) {
        printf("Erreur devcreation de texture du joueur : %s\n", SDL_GetError());
    }
    return texture;
}

/**
 * @fn void movePlayer (Player *p, Scene *s, int u, int d, int l, int r, unsigned int dt)
 * @param p joueur
 * @param s scene
 * @param u haut
 * @param d bas
 * @param l gauche
 * @param r droite
 * @param dt delta
 * @brief Deplace le joueur
 * @sa movePlayerToWorld()
*/
void movePlayer (Player *p, Scene *s, int u, int d, int l, int r, unsigned int dt) {


    if (p->touch_ground) {
        p->p0x = p->hitbox.x;
        p->p0y = p->hitbox.y;
        p->d0y = 0;
        p->flight_time = 0.0;
    }


    if (p->playable) {
    
        if (l) {
            p->dx = -(p->moving_speed) * (dt / 1000.0);
            movePlayerToWorld(p, s, p->dx, 0.0);
            
        }

        if (r) {
            p->dx = p->moving_speed * (dt / 1000.0);
            movePlayerToWorld(p, s, p->dx, 0.0);
        }

        if (u && p->touch_ground) {
            p->jumping = 1;
            p->d0y = -600;
            p->flight_time = 0.0;
        }
    }


    int player_mapX = p->hitbox.x / TILE_SIZE;
    int player_mapY = p->hitbox.y / TILE_SIZE;


    // Teleporteur
    if (s->map[player_mapY][player_mapX] == 4 || 
    s->map[player_mapY + 1][player_mapX] == 4 ||
    s->map[player_mapY][player_mapX + 1] == 4 ||
    s->map[player_mapY + 1][player_mapX + 1] == 4) {
        p->hitbox.x = 0;
        p->hitbox.y = 5 * TILE_SIZE;
        p->p0y = p->hitbox.y;
    }


    p->touch_ground = 0;


    float t = p->flight_time / 1000.0;
    p->flight_time += dt;

    float newY = (p->ay) * t * t / 2 + p->d0y * t + p->p0y;
    float dy = newY - (float)p->hitbox.y;
    p->dy = dy;
    movePlayerToWorld(p, s, 0.0, dy);

    // p->hitbox.y += (int)dy;

    // int pmx = (p->hitbox.x + s->camera.x) / TILE_SIZE;        
    // int pmy = p->hitbox.y + p->hitbox.h + s->camera.y;
    // pmy /= TILE_SIZE;

    // printf("%d, %d\n", pmx, pmy);

    // if (s->map[pmy][pmx] != 0) {
    //     p->hitbox.y = (pmy * TILE_SIZE) - p->hitbox.h + s->camera.y;
    //     p->touch_ground = 1;
    //     p->jumping = 0;
    //     p->flight_time = 0.0;
    // }


    // p->hitbox.y = (int)newY;

    // printf("%f %f %f\n", t, newY, dy);

    p->hitbox.x = i_min(i_max(p->hitbox.x, 0), s->map_width * TILE_SIZE - p->hitbox.w);
    p->hitbox.y = i_min(i_max(p->hitbox.y, 0), s->map_height * (TILE_SIZE - 1) - p->hitbox.h);

    
}

/**
 * @fn void movePlayerToWorld(Player *p, Scene *s, float dx, float dy)
 * @param p joueur
 * @param s scene
 * @param dx deplacement x
 * @param dy deplacement y
*/
void movePlayerToWorld(Player *p, Scene *s, float dx, float dy) {

    int player_mapX, player_mapY, hit, adj;

    if (dx != 0.0) {

        // On deplace le joueur
        p->hitbox.x += (int)dx;

        // Coordonees x dans la carte du point du cote qui avance
        // Point d'origine si on se deplace a gauche,
        // Point superieur droit si on se deplace a droite
        player_mapX = dx > 0 ? (p->hitbox.x + p->hitbox.w) : (p->hitbox.x);
        player_mapX /= TILE_SIZE;
        // printf("%d\n", player_mapX);

        player_mapY = (p->hitbox.y) / TILE_SIZE;
        // printf("%d\n", player_mapY);

        hit = 0;

        // On regarde si on touche la carte
        if (isInsideMap(s, player_mapX, player_mapY)) { // s->map[player_mapY][player_mapX] != 0
            hit = 1;
            // printf("x_hit\n");
        }

        // On reagrde le cote bas du bloc pour ne pas rentrer sur des parties de blocs
        // notamment en tombant et en se deplacant
        player_mapY = (p->hitbox.y + p->hitbox.h - 1) / TILE_SIZE;

        if (isInsideMap(s, player_mapX, player_mapY)) { // s->map[player_mapY][player_mapX] != 0
            hit = 1;
            // printf("x_hitt\n");
        }

        if (hit) {

            adj = dx > 0 ? -p->hitbox.w : TILE_SIZE;

            p->hitbox.x = (player_mapX * TILE_SIZE) + adj;

            // p->dx = 0;
        }




    }   
    if (dy != 0.0) {
        
        // On deplace le joueur
        p->hitbox.y += (int)dy;

        // Coordonees y dans la carte du point du cote qui avance

        player_mapY = dy > 0 ? (p->hitbox.y + p->hitbox.h) : (p->hitbox.y);
        player_mapY /= TILE_SIZE;
        // printf("player_mapY %d\n", player_mapY);

        player_mapX = (p->hitbox.x) / TILE_SIZE;
        // printf("player_mapX %d\n", player_mapX);

        hit = 0;

        // On regarde si on touche la carte
        if (isInsideMap(s, player_mapX, player_mapY)) { // s->map[player_mapY][player_mapX] != 0
            hit = 1;
            // printf("y_hit\n");
        }

        // On regarde si le joueur est entre deux blocs, ici pour le bloc de droite
        player_mapX = (p->hitbox.x + p->hitbox.w - 1) / TILE_SIZE;

        if (isInsideMap(s, player_mapX, player_mapY)) { // s->map[player_mapY][player_mapX] != 0
            hit = 1;
            // printf("y_hitt\n");
        }

        if (hit) {

            // if (p->dy < 0) {
            //     p->dy *= -1;
            //     adj = TILE_SIZE;
            // }
            // else {
            //     p->dy = 0;
            //     adj = -(p->hitbox.h);
            // }

            // p->p0y = p->hitbox.y;

            adj = dy > 0 ? -(p->hitbox.h) : TILE_SIZE;

            p->hitbox.y = (player_mapY * TILE_SIZE) + adj;

            
            p->d0y = dy > 0 ? 0 : p->d0y;

            // p->dy = 0;

            p->touch_ground = dy > 0;

            p->flight_time = dy > 0 ? 0.0 : p->flight_time;
            

        }

    }
}

/**
 * @fn void showPlayer (SDL_Renderer *renderer, Scene *scene, Player *player)
 * @param renderer rendu
 * @param scene scene
 * @param player joueur
 * @brief Montre le joueur
*/
void showPlayer (SDL_Renderer *renderer, Scene *scene, Player *player) {
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

    SDL_Rect screen_position = {0, 0, TILE_SIZE, TILE_SIZE};

    screen_position.x = player->hitbox.x - scene->camera.x;
    screen_position.y = player->hitbox.y - scene->camera.y;

    SDL_RenderCopy(renderer, player->texture, NULL, &screen_position);


    // SDL_RenderFillRect(renderer, &screen_position);

}

/**
 * @fn void doPlayer (SDL_Renderer *renderer, Player *player, Scene *scene, EntityList *entity_list, SDL_Texture **font_texture, int up, int down, int left, int right, unsigned int dt)
 * @param renderer rendu
 * @param player joueur
 * @param scene scene
 * @param entity_list liste des entites
 * @param font_texture tableau des textures
 * @param up haut
 * @param down bas
 * @param left gauche
 * @param right droite
 * @param dt delta
 * @brief Effectue les actions liees au joueur
*/
EntityList *doPlayer (SDL_Renderer *renderer, Player *player, Scene *scene, EntityList *entity_list, SDL_Texture **font_texture, int up, int down, int left, int right, unsigned int dt) {
    
    // Reduction du temps d'invincibilite
    
    // printf("%f\n", player->invincibility_time);
    if (player->invincibility_time > 0.0) {
        player->invincibility_time -= dt / 1000.0;
        if (player->invincibility_time < 0.0) player->invincibility_time = 0.0;
    }

    // Mouvement du joueur
    movePlayer(player, scene, up, down, left, right, dt);

    // regarder si on saute sur l'ennemi

    EntityList *jump_on = checkEntityCollisionDt(entity_list, player);


    if (jump_on != NULL) {
        if (!jump_on->entity.passive) {
            jump_on->entity.hp--;
            // printf("%d pv\n", jump_on->entity.hp);
            if (jump_on->entity.hp <= 0)  {
                printf("%s mort\n", jump_on->entity.name);
                entity_list = killEntity(entity_list, jump_on);
                // free(jump_on);
            }
        }
    }

    player->available_interaction = 0;

    // regarder si on touche l'ennnemi

    Entity *collide_with = checkEntityCollision(entity_list, player);

    if (collide_with != NULL) {
        if (collide_with->passive) {
            player->available_interaction = 1;

            // displayFont(renderer, font_texture, "E", collide_with->hitbox.x, collide_with->hitbox.h - collide_with->hitbox.h * 2 - 10, TILE_SIZE);
        } else {
            // Degats infilges
            if (player->invincibility_time <= 0.0) {
                player->hp--;
                player->invincibility_time = 2.0;
                printf("Degat inflige par %s, %d points de vie restants\n", collide_with->name, player->hp);
            }
        
        }
    }


    player->dx = 0;
    player->dy = 0;

    return entity_list;
}

/**
 * @fn Entity *checkEntityCollision (EntityList *entity_list, Player *player)
 * @param entity_list liste des entites
 * @param player joueur
 * @returns un pointeur vers l'entite, ou NULL sinon
 * @brief Teste si il y a une collision avec une entite
*/
Entity *checkEntityCollision (EntityList *entity_list, Player *player) {

    if (!isEmptyEntityList(entity_list)) {
        EntityList *i = NULL;
        for (i = entity_list; i != NULL; i = i->next) {
            if (SDL_HasIntersection(&(player->hitbox), &(i->entity.hitbox))) {
                // printf("Collision avec %s\n", i->entity.name);
                return &(i->entity);
            }
        }
    }

    return NULL;
}

/**
 * @fn Entity *checkEntityCollisionDt (EntityList *entity_list, Player *player)
 * @param entity_list liste des entites
 * @param player joueur
 * @returns un pointeur vers l'entite, ou NULL sinon
 * @brief Teste si il y a une collision avec une entite en fonction du deplacement vertical du joueur
*/
EntityList *checkEntityCollisionDt (EntityList *entity_list, Player *player) {

    if (!isEmptyEntityList(entity_list) && player->dy > 0) {
        EntityList *i = NULL;
        SDL_Rect nextPos = {player->hitbox.x, player->hitbox.y + player->dy, player->hitbox.w, player->hitbox.h};
        for (i = entity_list; i != NULL; i = i->next) {

            if (SDL_HasIntersection(&nextPos, &(i->entity.hitbox)) && player->hitbox.y + player->hitbox.h < i->entity.hitbox.y + 10) {
                // printf("Collision avec %d\n", i->entity.id);
                return i;
            }
        }
    }

    return NULL;
}


/* -------------------------------------------------- */
// Entites
/* -------------------------------------------------- */




/**
 * @fn void initEntity (Entity *entity, int id, int x, int y, int hp, int passive, const char* entity_name, int t_id, int dialogue_id)
 * @param entity entite
 * @param id identifiant
 * @param x position x
 * @param y position y
 * @param hp points de vie
 * @param passive entite passive
 * @param entity_name nom de l'entite
 * @param t_id id de la texture 
 * @param dialogue_id id du dialogue lors de l'interaction
 * @brief Initialise une entite
*/
void initEntity (Entity *entity, int id, int x, int y, int hp, int passive, const char* entity_name, int t_id, int dialogue_id) {
    entity->id = id;
    entity->hitbox.x = x;
    entity->hitbox.y = y;
    entity->hitbox.w = TILE_SIZE;
    entity->hitbox.h = TILE_SIZE;
    entity->hp = hp;
    entity->passive = passive;
    entity->dialogue_id = dialogue_id;
    entity->textureId = t_id;
    entity->name = malloc(sizeof(char) * (strlen(entity_name) + 1));
    if (entity->name != NULL) {
        entity->name[0] = '\0';
        sprintf(entity->name, entity_name); 
    }

}


/**
 * @fn SDL_Texture *loadEntityTexture(SDL_Renderer *renderer, const char *file_name)
 * @param renderer rendu
 * @param file_name nom du fichier
 * @returns un pointeur vers une texture
 * @brief Charge une texture d'entite
*/
SDL_Texture *loadEntityTexture(SDL_Renderer *renderer, const char *file_name) {
    SDL_Surface *image;
    SDL_Texture *texture;

    image = SDL_LoadBMP(file_name);
    if (image == NULL) {
        printf("Erreur de chargement d'image %s : %s\n", file_name, SDL_GetError());
    }

    texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);
    if (texture == NULL) {
        printf("Erreur chargement texture %s : %s\n", file_name, SDL_GetError());
    }

    return texture;

}

/**
 * @fn SDL_Texture **loadEntityTextures(SDL_Renderer *renderer)
 * @param renderer rendu
 * @returns un tableau de pointeurs sur textures
 * @brief Charge un tableau de textures
*/
SDL_Texture **loadEntityTextures(SDL_Renderer *renderer) {

    char file_name[MAX_FILENAME_LENGTH];

    SDL_Texture **texture_arr = malloc(sizeof(*texture_arr) * ENTITY_TEXTURE_COUNT);

    if (texture_arr == NULL) {
        printf("Erreur allocation memoire pour les textures des entites\n");
    }

    for (int i = 1; i <= ENTITY_TEXTURE_COUNT; i++) {
        sprintf(file_name, "ressources/entity%d.bmp", i);

        texture_arr[i - 1] = loadEntityTexture(renderer, file_name);
    }

    return texture_arr;
}

/**
 * @fn void unloadEntityTextures (SDL_Texture **texture_arr)
 * @param texture_arr tableau de textures
 * @brief Dechrage les textures des entites
*/
void unloadEntityTextures (SDL_Texture **texture_arr) {
    for (int i = 0; i < ENTITY_TEXTURE_COUNT; i++) {
        SDL_DestroyTexture(texture_arr[i]);
    }
    free(texture_arr);
    texture_arr = NULL;
}

/**
 * @fn void showEntity (SDL_Renderer *renderer, Scene *scene, SDL_Texture **texture_arr, Entity *entity)
 * @param renderer rendu
 * @param scene scene
 * @param texture_arr tableau de textures des entites
 * @param entity entite a afficher
 * @brief Affiche une entite sur le carte
*/
void showEntity (SDL_Renderer *renderer, Scene *scene, SDL_Texture **texture_arr, Entity *entity) {
    SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};

    rect.x = entity->hitbox.x - scene->camera.x;
    rect.y = entity->hitbox.y - scene->camera.y;

    if (entity->textureId > 0) {
        SDL_RenderCopy(renderer, texture_arr[entity->textureId - 1], NULL, &rect);
    }
}

/**
 * @fn void showEntities (SDL_Renderer *renderer, Scene *scene, SDL_Texture **texture_arr, EntityList *entity_list)
 * @param renderer rendu
 * @param scene scene
 * @param texture_arr tableau de textures des entites
 * @param entity_list liste des entites
 * @brief Affiche toutes les entites sur le carte
*/
void showEntities (SDL_Renderer *renderer, Scene *scene, SDL_Texture **texture_arr, EntityList *entity_list) {
    EntityList *list_element;

    for (list_element = entity_list; list_element != NULL; list_element = list_element->next) {
        showEntity(renderer, scene, texture_arr, &(list_element->entity));
    }
    // for (list_element = entity_list; list_element != NULL; list_element = list_element->next) {
    //     if (list_element->entity.hp > 0) {
    //         showEntity(renderer, scene, texture_arr, &(list_element->entity));
    //     }
    // }
}

/**
 * @fn void talkToEntity (Player *player, char ***dialogues, Entity *entity, MessageBox *message_box)
 * @param player joueur
 * @param dialogues tableau de dialogues
 * @param entity entite
 * @param message_box boite de messsage
 * @brief Parle a une entite
*/
void talkToEntity (Player *player, char ***dialogues, Entity *entity, MessageBox *message_box) {
    

    if (entity->dialogue_id != -1) {
        // int i = 0;
        // while (strcmp(dialogues[e->dialogue_id][i], "<") != 0) {
        //     printf("%s\n", dialogues[e->dialogue_id][i]);
        //     i++;
        // }


        player->playable = 0;

    

        // On ajoute le dialogue a la boite de mesage qui sera affichee
        message_box->used = 1;
        message_box->font_size = FONT_SIZE_INTERFACE;

        // Nom de l'entite
        message_box->speaker = entity->name;

        // Message
        message_box->message = dialogues[entity->dialogue_id][message_box->message_index];


        // si on a atteint la fin du dialogue
        if (strcmp(dialogues[entity->dialogue_id][message_box->message_index], "<") == 0) {
            player->playable = 1;
            message_box->used = 0;
            message_box->message_index = 0;
            message_box->speaker = NULL;
            message_box->message = NULL;
            printf("fin de dialogue\n");
        } else message_box->message_index++;



    } else printf("je parle\n");
}

EntityList *killEntity (EntityList *entity_list, EntityList *entity) {
    entity_list = removeEntityFromList(entity_list, entity);
    // printf("Entite tuee\n");
    return entity_list;
}



/*

void moveEntities (EntityList *entity_list, Scene *s, unsigned int dt) {

    EntityList *i = NULL;

    for (i = entity_list; i != NULL; i = i->next) {

        if (i->entity.touch_ground == 0) {
            float t = i->entity.flight_time / 1000.0;
            i->entity.flight_time += dt;

            float newY = (i->entity.ay) * t * t / 2 + i->entity.d0y * t + i->entity.p0y;
            float dy = newY - (float)i->entity.hitbox.y;
            i->entity.dy = dy;
            moveEntityToWorld(&(i->entity), s, 0.0, dy);
        }
    }
}


void moveEntityToWorld(Entity *e, Scene *s, float dx, float dy) {

    int player_mapX, player_mapY, hit, adj;

    if (dx != 0.0) {

        // On deplace le joueur
        e->hitbox.x += (int)dx;

        // Coordonees x dans la carte du point du cote qui avance
        // Point d'origine si on se deplace a gauche,
        // Point superieur droit si on se deplace a droite
        player_mapX = dx > 0 ? (e->hitbox.x + e->hitbox.w) : (e->hitbox.x);
        player_mapX /= TILE_SIZE;
        // printf("%d\n", player_mapX);

        player_mapY = (e->hitbox.y) / TILE_SIZE;
        // printf("%d\n", player_mapY);

        hit = 0;

        // On regarde si on touche la carte
        if (isInsideMap(s, player_mapX, player_mapY)) { // s->map[player_mapY][player_mapX] != 0
            hit = 1;
            // printf("x_hit\n");
        }

        // On reagrde le cote bas du bloc pour ne pas rentrer sur des parties de blocs
        // notamment en tombant et en se deplacant
        player_mapY = (e->hitbox.y + e->hitbox.h - 1) / TILE_SIZE;

        if (isInsideMap(s, player_mapX, player_mapY)) { // s->map[player_mapY][player_mapX] != 0
            hit = 1;
            // printf("x_hitt\n");
        }

        if (hit) {

            adj = dx > 0 ? -e->hitbox.w : TILE_SIZE;

            e->hitbox.x = (player_mapX * TILE_SIZE) + adj;

            // e->dx = 0;
        }




    }   
    if (dy != 0.0) {
        
        // On deplace le joueur
        e->hitbox.y += (int)dy;

        // Coordonees y dans la carte du point du cote qui avance

        player_mapY = dy > 0 ? (e->hitbox.y + e->hitbox.h) : (e->hitbox.y);
        player_mapY /= TILE_SIZE;
        // printf("player_mapY %d\n", player_mapY);

        player_mapX = (e->hitbox.x) / TILE_SIZE;
        // printf("player_mapX %d\n", player_mapX);

        hit = 0;

        // On regarde si on touche la carte
        if (isInsideMap(s, player_mapX, player_mapY)) { // s->map[player_mapY][player_mapX] != 0
            hit = 1;
            // printf("y_hit\n");
        }

        // On regarde si le joueur est entre deux blocs, ici pour le bloc de droite
        player_mapX = (e->hitbox.x + e->hitbox.w - 1) / TILE_SIZE;

        if (isInsideMap(s, player_mapX, player_mapY)) { // s->map[player_mapY][player_mapX] != 0
            hit = 1;
            // printf("y_hitt\n");
        }

        if (hit) {

            // if (e->dy < 0) {
            //     e->dy *= -1;
            //     adj = TILE_SIZE;
            // }
            // else {
            //     e->dy = 0;
            //     adj = -(e->hitbox.h);
            // }

            // e->p0y = e->hitbox.y;

            adj = dy > 0 ? -(e->hitbox.h) : TILE_SIZE;

            e->hitbox.y = (player_mapY * TILE_SIZE) + adj;

            
            e->d0y = dy > 0 ? 0 : e->d0y;

            // e->dy = 0;

            e->touch_ground = dy > 0;

            e->flight_time = dy > 0 ? 0.0 : e->flight_time;
            

        }

    }
}

*/

/**
 * @fn void doEntities (Scene *scene, EntityList *entity_list, unsigned int dt)
 * @brief Effectue les actions liees aux entite
*/
void doEntities (Scene *scene, EntityList *entity_list, unsigned int dt) {
    // moveEntities(entity_list, scene, dt);
}

/**
 * @fn EntityList *loadPassiveEntities(EntityList *entity_list, unsigned int *entity_id)
 * @param entity_list liste des entites
 * @param entity_id identifiant global des entites
 * @returns la liste chainee des entites
 * @brief Charge les entites passives dans la liste
*/
EntityList *loadPassiveEntities(EntityList *entity_list, unsigned int *entity_id) {
    Entity e;
    int map_positions[4][2] = {{1, 27}, {55, 27}, {94, 27}, {13, 57}};

    char names[4][10] = {
        {"PEDRO"}, {"BERNARD"}, {"PLOUCOS"}, {"LE NUL"}
    };

    
    int dialogues_ids[4] = {0, 1, 2, 3};

    for (int i = 0; i < 4; i++) {
        initEntity(&e, *entity_id, map_positions[i][0] * TILE_SIZE, map_positions[i][1] * TILE_SIZE, 1, 1, names[i], 2, dialogues_ids[i]);
        (*entity_id)++;
        entity_list = addEntityToFrontList(entity_list, e);
        printf("entite ajoute\n");
    }

    printf("pancatre\n");
    
    // Pancarte
    initEntity(&e, *entity_id, 6 * TILE_SIZE, 27 * TILE_SIZE, 1, 1, "PANCATRE", 3, 4);
    (*entity_id)++;
    entity_list = addEntityToFrontList(entity_list, e);
    printf("entite ajoute\n");




    return entity_list;
}

/**
 * @fn EntityList *loadHostileEntities(EntityList *entity_list, unsigned int *entity_id)
 * @param entity_list liste des entites
 * @param entity_id identifiant global des entites
 * @returns la liste chainee des entites
 * @brief Charge les entites hostiles dans la liste
*/
EntityList *loadHostileEntities(EntityList *entity_list, unsigned int *entity_id) {
    Entity e;
    int map_positions[4][2] = {{8, 27}, {10, 27}, {98, 27}, {18, 57}};

    char names[4][10] = {
        {"MECHANT"}, {"MECHANT"}, {"MECHANT"}, {"MECHANT"}
    };


    for (int i = 0; i < 4; i++) {
        initEntity(&e, *entity_id, map_positions[i][0] * TILE_SIZE, map_positions[i][1] * TILE_SIZE, 1, 0, names[i], 1, -1);
        (*entity_id)++;
        entity_list = addEntityToFrontList(entity_list, e);
    }

    return entity_list;
}






// Listes chainees


/**
 * @fn EntityList *createEntityList ()
 * @param e Entite
 * @returns NULL pour creer la liste
 * @brief Cree une liste chainee
*/
EntityList *createEntityList (Entity e) {
    EntityList *entity_list = NULL;
    entity_list = malloc(sizeof(EntityList));
    if (!entity_list) {
        printf("ERREUR allocation malloc creation entite\n");
        // system("pause");
        exit(EXIT_FAILURE);
    }
    entity_list->entity = e;
    entity_list->next = NULL;
    return entity_list;
}

/**
 * @fn int isEmptyEntityList (EntityList *entity_list)
 * @param entity_list liste des entites
 * @returns 1 si la liste est vide, 0 sinon
 * @brief Indique si la liste chainee est vide ou non
*/
int isEmptyEntityList (EntityList *entity_list) {
    return !entity_list;
}

/**
 * @fn int entityCount (EntityList *entity_list)
 * @param entity_list liste des entites
 * @returns le nombre d'elements de la liste
 * @brief 
*/
int entityCount (EntityList *entity_list) {
    int nb = 0;
    for (EntityList *i = entity_list; i != NULL; i = i->next) {
        nb++;
    }
    return nb;
}

/**
 * @fn EntityList *addEntityToFrontList (EntityList *entity_list, Entity element)
 * @param entity_list liste des entites
 * @param element element a retirer
 * @returns la liste chainee des entites
 * @brief Ajoute une entite au debut de la liste
*/
EntityList *addEntityToFrontList (EntityList *entity_list, Entity element) {
    EntityList *e = createEntityList(element);
    e->next = entity_list;
    return e;
}

/**
 * @fn EntityList *addEntityToBackList (EntityList *entity_list, Entity element)
 * @param entity_list liste des entites
 * @param element element a retirer
 * @returns la liste chainee des entites
 * @brief Ajoute une entite a la fin de la liste
*/
EntityList *addEntityToBackList (EntityList *entity_list, Entity element) {

    EntityList *e = createEntityList(element);

    if (isEmptyEntityList(entity_list)) {
        return e;
    }

    EntityList *i = entity_list;
    while (i->next != NULL) {
        i = i->next;
    }
    i->next = e;

    return entity_list;
}

/**
 * @fn EntityList *removeEntityFrontList (EntityList *entity_list)
 * @param entity_list liste des entites
 * @returns la liste des entites
 * @brief Enleve l'element en debut de liste
*/
EntityList *removeEntityFrontList (EntityList *entity_list) {
    if (isEmptyEntityList(entity_list)) return NULL;

    EntityList *temp = entity_list;
    entity_list = entity_list->next;
    free(temp);

    return entity_list;
}

/**
 * @fn EntityList *removeEntityBackList (EntityList *entity_list)
 * @param entity_list liste des entites
 * @returns la liste des entites
 * @brief Enleve l'element en fin de liste
*/
EntityList *removeEntityBackList (EntityList *entity_list) {
    if (isEmptyEntityList(entity_list)) return NULL;

    EntityList *p = NULL;
    EntityList *i = entity_list;

    // si un seul element
    if (!(entity_list->next)) {
        free(entity_list);
        return NULL;
    }

    while (i->next) {
        p = i;
        i = i->next;

    }

    free(i);
    p->next = NULL;

    return entity_list;
}

/**
 * @fn EntityList *removeEntityFromList (EntityList *entity_list, EntityList *entity)
 * @param entity_list liste des entites
 * @param entity l'entite a retirer
 * @returns la liste des entites
 * @brief Enleve un element a n'importe quelle position dans la liste
 * @remarks Bug mais ca a l'air de marcher
*/
EntityList *removeEntityFromList (EntityList *entity_list, EntityList *entity) {
    if (isEmptyEntityList(entity_list)) return NULL;

    // Regarder le premier emplacement
    if (entity_list == entity) {
        entity_list = removeEntityFrontList(entity_list);
        printf("Entite retiree du debut de liste\n");
        return entity_list;
    }
    // Verifier si c'est le dernier element

    EntityList *i = entity_list;
    EntityList *prev = entity_list;


    // Verifier si c'est le dernier element ?????
    if (entity->next == NULL) return removeEntityBackList(entity_list);

    for (i = entity_list; i != NULL; i = i->next) {
        if (i == entity) {

            prev->next = i->next;
            free(i);
            i = NULL;
            // showEntityList(entity_list);
            // printf("Entite retiree de la liste\n");
            return entity_list;
        }


        prev = i;
    }

    // showEntityList(entity_list);
    return entity_list;
}

/**
 * @fn EntityList *clearEntityList (EntityList *entity_list)
 * @param entity_list liste des entites
 * @returns la liste des entites
 * @brief Nettoie la liste des entites
*/
EntityList *clearEntityList (EntityList *entity_list) {

    if (!isEmptyEntityList(entity_list)) return NULL;
    while (!isEmptyEntityList(entity_list)) {
        entity_list = removeEntityFrontList(entity_list);
    }
    return entity_list;
}

/**
 * @fn void showEntityList (EntityList *entity_list)
 * @param entity_list liste des entites
 * @brief Affiche la liste des entites
*/
void showEntityList (EntityList *entity_list) {
    if (isEmptyEntityList(entity_list)) {
        printf("Liste vide\n");
        return;
    }

    printf("-----------------\n");
    for (EntityList *i = entity_list; i != NULL; i = i->next) {
        printf("Element : %s\n", i->entity.name);
    }
    printf("-----------------\n");
}

/**
 * @fn EntityList *addEntityToPlayerPos (EntityList *entity_list, Player *player, unsigned int id)
 * @param entity_list liste des entites
 * @param player joueur
 * @param id id global des entites
*/
EntityList *addEntityToPlayerPos (EntityList *entity_list, Player *player, unsigned int id) {
    Entity e;
    // printf("init de e\n");
    char name[10];
    sprintf(name, "Pedro%d", id);
    initEntity(&e, id, player->hitbox.x, player->hitbox.y, 1, 0, name, 1, -1);
    // printf("e init\n");
    // printf("%s\n", e.name);
    entity_list = addEntityToFrontList(entity_list, e);
    printf("Entite ajoutee\n");
    player->invincibility_time = 0.5;
    return entity_list;
}



