//
// Created by sadegh on 1/28/19.
//

#include "saveandload.h"
#include "structs.h"
#include "view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

int purpose_load_number = 1;

void save_game(Map *map) {

    //knowing the number of saves before it
    FILE *file_of_number = fopen("saves/numberofsaves.txt", "r");
    int numberofsaves;
    fscanf(file_of_number, "%d", &numberofsaves);
    fclose(file_of_number);
    //end

    //save the new number of saves
    numberofsaves++;
    file_of_number = fopen("saves/numberofsaves.txt", "w");
    fprintf(file_of_number, "%d", numberofsaves);
    fclose(file_of_number);
    //end

    //preparing address of new save
    FILE *check_nevis = fopen("saves/checknevis.txt", "w");
    fprintf(check_nevis, "saves/save_%d", numberofsaves);
    fclose(check_nevis);
    check_nevis = fopen("saves/checknevis.txt", "r");
    char address_of_save[20];
    fscanf(check_nevis, "%s", address_of_save);
    fclose(check_nevis);

    //make a file for new save
    FILE *new_file_save = fopen(address_of_save, "w");

    //save game information
    {
        //map information
        fprintf(new_file_save, "%d %lld\n", map->number_of_walls, map->frames);

        //walls information
        for (int i = 0; i < map->number_of_walls; ++i) {
            Wall *wall = &(map->walls[i]);
            fprintf(new_file_save, "%d %d %d %d %d\n", wall->x1, wall->y1, wall->x2, wall->y2, wall->vertical);
        }

        //tanks information
        for (int i = 0; i < 3; ++i) {
            Tank *tank = &(map->tank[i]);
            fprintf(new_file_save, "%d %d %d %f\n", tank->x, tank->y, tank->score, tank->angle);
            fprintf(new_file_save, "%d %d %d %d\n", tank->is_alive, tank->powered_up, tank->frag_section, tank->have_mine);

            //Frag bomb
            {
                fprintf(new_file_save, "%d\n", tank->fragBomb.is_released);
                //save information of bullets of frag bomb
                for (int j = 0; j < 9; ++j) {
                    Bullet *bullet = &(tank->fragBomb.bullet[j]);
                    save_bullet_information(new_file_save, bullet);
                }
            }

            //Tank bullets
            {
                for (int j = 0; j < 6; ++j) {
                    Bullet *bullet = &(tank->bullet[j]);
                    save_bullet_information(new_file_save, bullet);
                }
            }

            //color of tank
            fprintf(new_file_save, "%d %d %d\n", tank->color[0], tank->color[1], tank->color[2]);
        }

        //Powerups
        for (int i = 0; i < 5; ++i) {
            Powerup *powerup = &(map->powerup[i]);
            fprintf(new_file_save, "%d %d %d %d %d\n", powerup->x, powerup->y, powerup->type, powerup->use_by, powerup->is_on);
        }

        //Mines
        for (int i = 0; i < 15; ++i) {
            Mine *mine = &(map->mine[i]);
            fprintf(new_file_save, "%d %d %d %d %d %d\n", mine->x, mine->y, mine->is_planted, mine->frame_from_born, mine->is_in_range, mine->frame_from_in_range);
        }
    }

    ///dont forget
    //key pressed initialization at loading
    ///dont forget

    fclose(new_file_save);
}

void save_bullet_information(FILE *file, Bullet *bullet) {
    fprintf(file, "%d %d %d %f %d %d\n", bullet->in_wall, bullet->x, bullet->y, bullet->angle, bullet->is_fired, bullet->remained_life);
}

int number_of_saves() {
    FILE *file_of_number = fopen("saves/numberofsaves.txt", "r");
    int numberofsaves;
    fscanf(file_of_number, "%d", &numberofsaves);
    fclose(file_of_number);
    return numberofsaves;
}

void load_game(Map *map, int load_number) {
    //knowing the number of saves before it
    FILE *file_of_number = fopen("saves/numberofsaves.txt", "r");
    int numberofsaves;
    fscanf(file_of_number, "%d", &numberofsaves);
    fclose(file_of_number);
    //end


    //preparing address of new load
    FILE *check_nevis = fopen("saves/checknevis.txt", "w");
    fprintf(check_nevis, "saves/save_%d", load_number);
    fclose(check_nevis);
    check_nevis = fopen("saves/checknevis.txt", "r");
    char address_of_save[20];
    fscanf(check_nevis, "%s", address_of_save);
    fclose(check_nevis);


    //reading file for load
    FILE *load_file = fopen(address_of_save, "r");

    //load game information
    {
        //map information
        fscanf(load_file, "%d %lld\n", &map->number_of_walls, &map->frames);

        //walls information
        for (int i = 0; i < map->number_of_walls; ++i) {
            Wall *wall = &(map->walls[i]);
            fscanf(load_file, "%d %d %d %d %d\n", &wall->x1, &wall->y1, &wall->x2, &wall->y2, &wall->vertical);
        }

        //tanks information
        for (int i = 0; i < 3; ++i) {
            Tank *tank = &(map->tank[i]);
            fscanf(load_file, "%d %d %d %lf\n", &tank->x, &tank->y, &tank->score, &tank->angle);
            fscanf(load_file, "%d %d %d %d\n", &tank->is_alive, &tank->powered_up, &tank->frag_section, &tank->have_mine);

            //Frag bomb
            {
                fscanf(load_file, "%d\n", &tank->fragBomb.is_released);
                //save information of bullets of frag bomb
                for (int j = 0; j < 9; ++j) {
                    Bullet *bullet = &(tank->fragBomb.bullet[j]);
                    load_bullet_information(load_file, bullet);
                }
            }

            //Tank bullets
            {
                for (int j = 0; j < 6; ++j) {
                    Bullet *bullet = &(tank->bullet[j]);
                    load_bullet_information(load_file, bullet);
                }
            }

            //color of tank
            fscanf(load_file, "%d %d %d\n", &tank->color[0], &tank->color[1], &tank->color[2]);
        }

        //Powerups
        for (int i = 0; i < 5; ++i) {
            Powerup *powerup = &(map->powerup[i]);
            fscanf(load_file, "%d %d %d %d %d\n", &powerup->x, &powerup->y, &powerup->type, &powerup->use_by, &powerup->is_on);
        }

        //Mines
        for (int i = 0; i < 15; ++i) {
            Mine *mine = &(map->mine[i]);
            fscanf(load_file, "%d %d %d %d %d %d\n", &mine->x, &mine->y, &mine->is_planted, &mine->frame_from_born, &mine->is_in_range, &mine->frame_from_in_range);
        }
    }


    fclose(load_file);
}

void load_bullet_information(FILE *file, Bullet *bullet) {
    fscanf(file, "%d %d %d %lf %d %d\n", &bullet->in_wall, &bullet->x, &bullet->y, &bullet->angle, &bullet->is_fired, &bullet->remained_life);
}

int load_event_handling() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return EXIT;
        }
        if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
                case SDLK_DOWN:
                    purpose_load_number--;
                    if (purpose_load_number < 1) {
                        purpose_load_number = NOS;
                    }
                    break;
                case SDLK_UP:
                    purpose_load_number++;
                    if (purpose_load_number > NOS) {
                        purpose_load_number = 1;
                    }
                    break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    return purpose_load_number;
            }
        }
    }
    return 0;
}

int get_load_number(Map *map, SDL_Renderer *renderer) {
    random_line_place(map);
    while (1) {
        render_clear();
        draw_dancing_lines(map, renderer);
        roundedBoxRGBA(renderer, x_max / 2 + 250, 390, x_max / 2 + 320, 435, 10, 130, 125, 120, 255);
        SDL_RenderSetScale(renderer, 3, 3);
        stringRGBA(renderer, (x_max / 2 - 250) / 3, 200 / 3, "Number of saves:", 0, 0, 0, 255);
        stringRGBA(renderer, (x_max / 2 + 150) / 3, 200 / 3, convert_number_to_string(NOS), 0, 0, 0, 255);
        stringRGBA(renderer, (x_max / 2 - 340) / 3, 400 / 3, "You want to load number:", 0, 0, 0, 255);
        stringRGBA(renderer, (x_max / 2 + 260) / 3, 400 / 3, convert_number_to_string(purpose_load_number), 0, 0, 0, 255);
        SDL_RenderSetScale(renderer, 1, 1);
        int x = load_event_handling();
        if (x) {
            return x;
        }
        present_window();
    }
}