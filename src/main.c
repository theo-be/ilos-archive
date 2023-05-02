/** 
 * @file main.c
 * @version 0.2.1
 * @date 25/08/2022 Debut du projet
 * @brief Fichier principal du projet
*/


#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>



#include "main.h"
#include "structures.h"
#include "entities.h"
#include "variables.h"
#include "scene.h"
#include "font.h"
#include "interface.h"

// void foo (EntityList *entity_list) {
//     Entity entity1;
//     initEntity(&entity1, 1);
//     Entity entity2;
//     initEntity(&entity2, 2);
//     Entity entity3;
//     initEntity(&entity3, 3);
//     printf("%d\n", entityCount(entity_list));
//     showEntityList(entity_list);
//     printf("----------------------\n");
//     entity_list = addEntityToBackList(entity_list, entity1);
//     printf("%d\n", entityCount(entity_list));
//     showEntityList(entity_list);
//     printf("----------------------\n");
//     entity_list = addEntityToFrontList(entity_list, entity2);
//     printf("%d\n", entityCount(entity_list));
//     showEntityList(entity_list);
//     printf("----------------------\n");
//     entity_list = addEntityToBackList(entity_list, entity3);
//     printf("%d\n", entityCount(entity_list));
//     showEntityList(entity_list);
//     printf("----------------------\n");
//     entity_list = removeEntityBackList(entity_list);
//     entity_list = removeEntityFrontList(entity_list);
//     entity_list = removeEntityBackList(entity_list);
//     entity_list = clearEntityList(entity_list);
//     printf("%d\n", entityCount(entity_list));
//     showEntityList(entity_list);
// }



int main(int argc, char *argv[]) {
    printf("ilos v0.2.4\n");
    SDL_version version;
    SDL_VERSION(&version);
    printf("SDL v%d.%d.%d\n", version.major, version.minor, version.patch);

    int exit_status = EXIT_FAILURE;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    // int window_width = WINDOW_WIDTH;
    // int window_height = WINDOW_HEIGHT;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur initialisation SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if ((window = SDL_CreateWindow("ilos", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)) == NULL) { //  | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
        printf("Erreur initialisation fenetre : %s\n", SDL_GetError());
        exitSDL(window, renderer);
        exit(EXIT_FAILURE);
    }

    if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)) == NULL) {
        printf("Erreur initialisation rendu : %s\n", SDL_GetError());
        exitSDL(window, renderer);
        exit(EXIT_FAILURE);
    }





    /* -------------------------------------------------- */
    // Debut des initialisations
    // Initialisation de la scene
    /* -------------------------------------------------- */



    printf("Initialisation de la scene\n");
    Scene scene;
    initScene(&scene);
    printf("Scene initialisee, chargement de la carte\n");
    scene.map = loadMap("data/map", &scene.map_height, &scene.map_width);
    printf("Carte chargee\n");
    printf("Hauteur carte : %d\n", scene.map_height);
    printf("Longueur carte : %d\n", scene.map_width);
    // for (int i = 0; i < scene.map_height; i++) {
    //     for (int j = 0; j < scene.map_width; j++) {
    //         printf("%d,", scene.map[i][j]);
    //     }
    //     printf("\n");
    // }


    /* -------------------------------------------------- */
    // Initialisation des textures
    /* -------------------------------------------------- */


    printf("Chargement des textures\n");

    // Textures de la carte
    SDL_Texture **foreground_textures = loadTiles(renderer);
    printf("Textures de la carte chargees\n");

    // Textures des entites
    SDL_Texture **entity_textures = loadEntityTextures(renderer);
    printf("Textures des entites chargees\n");


    printf("Textures chargees\n");


    /* -------------------------------------------------- */
    // Initialisation de la police
    /* -------------------------------------------------- */
    printf("Initialisation de la police\n");

    SDL_Texture **font_textures = loadFont(renderer);


    printf("Police chargee\n");


    /* -------------------------------------------------- */
    // Initialisation du joueur
    /* -------------------------------------------------- */


    printf("Initialisation du joueur\n");
    Player player;
    initPlayer(renderer, &player);
    // player.texture = loadPlayerTexture(renderer);
    printf("Joueur initialise\n");
    // printf("%d, %d, %d hp\n", player.hitbox.x, player.hitbox.y, player.hp);

    /* -------------------------------------------------- */
    // Initialisation des entites
    /* -------------------------------------------------- */

    printf("Initialisation des entites\n");

    // Chaque entite a un id different, meme apres leur reapparition
    unsigned int entity_id = 0;
    
    EntityList *entity_list = NULL;
    // foo(entity_list);


    printf("chargement des entites passives\n");

    entity_list = loadPassiveEntities(entity_list, &entity_id);


    printf("chargement des entites hostiles\n");
    entity_list = loadHostileEntities(entity_list, &entity_id);



    printf("Entites initialisees\n");

    /* -------------------------------------------------- */
    // Initialisation des dialogues
    /* -------------------------------------------------- */

    printf("Initialisation des dialogues\n");


    // le code de chargement de dialogue
    
    char ***dialogues = loadDialogues();
    
    MessageBox dialogue = {NULL, NULL, 0, 0};
    
    
    
    
   printf("Dialogues initialises\n");

    /* -------------------------------------------------- */
    // Autres initialisations
    /* -------------------------------------------------- */

    int debug = 1;

    int up = 0, down = 0, left = 0, right = 0;

    unsigned int frame_limit = SDL_GetTicks() + F_DELTA;

    unsigned int fps_counter = 0;

    unsigned int dt = 0;
    unsigned int p_dt = 0;

    SDL_bool program_launched = SDL_TRUE;

    printf("Jeu totalement initialise\n");
    // printf("%d\n", FONT_TEXTURE_COUNT);

    /* -------------------------------------------------- */
    // Debut de la boucle infinie
    /* -------------------------------------------------- */



    while (program_launched) {

        dt = SDL_GetTicks() - p_dt;
        p_dt = SDL_GetTicks();

        fps_counter = 1000 / dt;

        /* Debut des evenements ------------------------------------------- */

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    program_launched = SDL_FALSE;
                    break;
                /* -------------------------------------------------- */
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_q:
                            left = 1;
                            break;
                        case SDLK_d:
                            right = 1;
                            break;
                        case SDLK_z:
                            up = 1;
                            break;
                        case SDLK_s:
                            down = 1;
                            break;
                        case SDLK_e:
                            // fonction d'interaction avec entite
                            if (player.available_interaction) {
                                playerInteraction(&player, entity_list, dialogues, &dialogue, 1);
                            }
                            break;
                        case SDLK_p:
                            program_launched = SDL_FALSE;
                            break;
                        default:
                            break;
                    }
                    break;
                /* -------------------------------------------------- */
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_q:
                            left = 0;
                            break;
                        case SDLK_d:
                            right = 0;
                            break;
                        case SDLK_z:
                            up = 0;
                            break;
                        case SDLK_s:
                            down = 0;
                            break;
                        // Debug
                        /* ------------------------------ */
                        case SDLK_b:
                            printf("interaction : %d\nboite utilisee : %d\n", player.available_interaction, dialogue.used);
                            break;
                        case SDLK_i:
                            player.playable = player.playable == 1 ? 0 : 1;
                            break;
                        case SDLK_k:
                            entity_list = removeEntityFrontList(entity_list);
                            break;
                        case SDLK_l:
                            entity_list = addEntityToPlayerPos(entity_list, &player, entity_id);
                            entity_id++;
                            // showEntityList(entity_list);
                            break;
                        case SDLK_m:
                            centerCameraToPlayer(&scene, &player);
                            break;
                        // case SDLK_n:
                        //     unloadDialogues(dialogues);
                        //     break;
                        case SDLK_o:
                            showEntityList(entity_list);
                            break;
                        case SDLK_v:
                            debug = !debug;
                            printf("debug : %d\n", debug);
                        
                        /* ------------------------------ */
                        default:
                            break;
                    }
                    break;
                /* -------------------------------------------------- */
                // case SDL_WINDOWEVENT_RESIZED:
                //     printf("maximisee\n");
                //     break;
                default:
                    break;
            }
        }

        /* Fin des evenements --------------------------------------------- */

        SDL_SetRenderDrawColor(renderer, 128, 192, 255, SDL_ALPHA_OPAQUE); // bleu clair
        SDL_RenderClear(renderer);




        entity_list = doPlayer(renderer, &player, &scene, entity_list, font_textures, up, down, left, right, dt);

        doEntities(&scene, entity_list, dt);        

        doScene(&scene, &player, up, down, left, right, dt);


        showMap(renderer, &scene, foreground_textures);
        showEntities(renderer, &scene, entity_textures, entity_list);
        showPlayer(renderer, &scene, &player);
        showInterface(renderer, font_textures, &scene, &player, &dialogue);



        // Informations de debug
        if (debug) {
            char fps[9] = {'\n'};
            char entities_count[30] = {0};
            sprintf(fps, "%dFPS", fps_counter);
            displayFont(renderer, font_textures, fps, 0, 0, 24);
            sprintf(entities_count, "%d ENTITES RESTANTES", entityCount(entity_list));
            displayFont(renderer, font_textures, entities_count, 0, 32, 24);
        }


        

        
        // displayFont(renderer, font_textures, "FE1SF1ES3F4ESF1ES5F41ESG4R3DJ4TY53J4UY53K", 0, 0, 24);
        // displayFont(renderer, font_textures, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0, 0, 24);
        // displayFont(renderer, font_textures, "0123456789", 26 * 24, 0, 24);
        

        SDL_RenderPresent(renderer);
        
        limitFPS(frame_limit);
        frame_limit = SDL_GetTicks() + F_DELTA;
    }
    

    /* Fin du programme -------------------------------------------------- */




    printf("Dechargement des dialogues\n");
    unloadDialogues(dialogues);
    printf("Dialogues decharges\n");


    printf("Dechargement de la police\n");
    unloadFont(font_textures);
    printf("Police dechargee\n");


    printf("Dechargement des entites\n");
    entity_list = clearEntityList(entity_list);
    printf("Entites dechargees\n");

    printf("Dechargement de la carte\n");
    unloadMap(scene.map, scene.map_height);
    printf("Carte dechargee\n");







    printf("Dechargement des textures\n");

    printf("Dechargement des textures de la carte\n");
    unloadTiles(foreground_textures);
    printf("Textures de la carte dechargees\n");

    printf("Dechargement des textures des entites\n");
    unloadEntityTextures(entity_textures);
    SDL_DestroyTexture(player.texture);
    printf("Textures des entites dechargees\n");

    printf("Textures dechargees\n");


    exitSDL(window, renderer);

    printf("SDL dechargee\n");

    exit_status = EXIT_SUCCESS;

    // system("pause");
    return exit_status;
}




/**
 * @fn void limitFPS (unsigned int limit)
 * @param limit limite
 * @brief Limite l'execution du programme pour avoir un taux d'images par seconde stable
 * @date 25/28/2022
*/
void limitFPS (unsigned int limit) {
    unsigned int ticks = SDL_GetTicks();

    if (limit < ticks) return;
    else if (limit > ticks + F_DELTA) SDL_Delay(F_DELTA);
    else SDL_Delay (limit - ticks);
}

/**
 * @fn void exitSDL (SDL_Window *w, SDL_Renderer *r)
 * @param r rendu
 * @param w fenetre
 * @brief Detruit le rendu et la fenetre et quitte SDL
 * @date 25/28/2022
*/
void exitSDL (SDL_Window *w, SDL_Renderer *r) {
    if (r != NULL) SDL_DestroyRenderer(r);
    if (w != NULL) SDL_DestroyWindow(w);
    SDL_Quit();
}





/*

corriger les structures
corriger la suppression debut liste chainee

remettre bien la conversion lors de la suppression des entites (si confirmation de non plantage)

arranger les structures


afficher les coeurs
affichage des entites uniquement sur le zone de rendu


*/




