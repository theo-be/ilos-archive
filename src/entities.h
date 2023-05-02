
#ifndef ENTITIES_H

#define ENTITIES_H

#include <SDL.h>
#include "structures.h"

// Joueur

void initPlayer (SDL_Renderer *r, Player *p);

SDL_Texture *loadPlayerTexture (SDL_Renderer *renderer);

void showPlayer (SDL_Renderer *renderer, Scene *scene, Player *player);

void movePlayer (Player *p, Scene *s, int u, int d, int l, int r, unsigned int dt);

void movePlayerToWorld(Player *p, Scene* s, float dx, float dy);

EntityList *doPlayer (SDL_Renderer *renderer, Player *player, Scene *scene, EntityList *entity_list, SDL_Texture **font_texture, int up, int down, int left, int right, unsigned int dt);

Entity *checkEntityCollision (EntityList *entity_list, Player *player);

EntityList *checkEntityCollisionDt (EntityList *entity_list, Player *player);

// Entites

void initEntity (Entity *entity, int id, int x, int y, int hp, int passive, const char* entity_name, int t_id, int dialogue_id);

SDL_Texture *loadEntityTexture (SDL_Renderer *renderer, const char *file_name);

SDL_Texture **loadEntityTextures (SDL_Renderer *renderer);


void unloadEntityTextures (SDL_Texture **texture_arr);

void showEntity (SDL_Renderer *renderer, Scene *scene, SDL_Texture **textures, Entity *entity);

void showEntities (SDL_Renderer *renderer, Scene *scene, SDL_Texture **texture_arr, EntityList *entity_list);

void moveEntities (EntityList *entity_list, Scene *s, unsigned int dt);

void moveEntityToWorld(Entity *e, Scene *s, float dx, float dy);

void doEntities (Scene *scene, EntityList *entity_list, unsigned int dt);

EntityList *loadPassiveEntities(EntityList *entity_list, unsigned int *entity_id);

EntityList *loadHostileEntities(EntityList *entity_list, unsigned int *entity_id);

EntityList *killEntity (EntityList *entity_list, EntityList *entity);

void talkToEntity (Player *player, char ***dialogues, Entity *entity, MessageBox *message_box);




EntityList *createEntityList (Entity e);

int isEmptyEntityList (EntityList *entity_list);

int entityCount (EntityList *entity_list);

EntityList *addEntityToFrontList (EntityList *entity_list, Entity element);

EntityList *addEntityToBackList (EntityList *entity_list, Entity element);

EntityList *removeEntityFrontList (EntityList *entity_list);

EntityList *removeEntityBackList (EntityList *entity_list);

EntityList *clearEntityList (EntityList *entity_list);

void showEntityList (EntityList *entity_list);

EntityList *removeEntityFromList (EntityList *entity_list, EntityList *entity);

EntityList *addEntityToPlayerPos (EntityList *entity_list, Player *player, unsigned int id);



#endif