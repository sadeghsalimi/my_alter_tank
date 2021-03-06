//
// Created by sadegh on 12/20/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "structs.h"
#include "view.h"
#include "physics.h"
#include "logic.h"
#include "playwav.h"

int vertex[12][7];
int dx[4] = {0, 0, 1, -1};
int dy[4] = {1, -1, 0, 0};
int win_score;

int exist_0_vertex() {
    int exist = 0;
    for (int i = 1; i < 11; ++i) {
        for (int j = 0; j < 6; ++j) {
            if (vertex[i][j] == 0) {
                exist = 1;
                break;
            }
        }
        if (exist) {
            break;
        }
    }
    return exist;
}

int is_wall_vertical(int x1, int x2) {
    if (x1 == x2) {
        return 1;
    }
    return 0;
}

void generate_map(Map *map) {
    srand((unsigned int)(time(NULL)));
    map->number_of_walls = 0;
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 7; ++j) {
            vertex[i][j] = 0;
        }
    }
    for (int i = 0; i < 12; i += 11) {
        for (int j = 0; j < 7; j++) {
            vertex[i][j] = 1;
        }
    }
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 7; j += 6) {
            vertex[i][j] = 1;
        }
    }
    //around_walls
    ///*****
    map->walls[0].x1 = 0; map->walls[0].y1 = 0; map->walls[0].x2 = 11; map->walls[0].y2 = 0; map->walls[0].vertical = 0;
    map->walls[1].x1 = 11; map->walls[1].y1 = 0; map->walls[1].x2 = 11; map->walls[1].y2 = 6; map->walls[1].vertical = 1;
    map->walls[2].x1 = 11; map->walls[2].y1 = 6; map->walls[2].x2 = 0; map->walls[2].y2 = 6; map->walls[2].vertical = 0;
    map->walls[3].x1 = 0; map->walls[3].y1 = 6; map->walls[3].x2 = 0; map->walls[3].y2 = 0; map->walls[3].vertical = 1;
    map->number_of_walls = 4;
    ///*****
    //around walls
//    if (!(rand() % 3)) {
//        map->walls[map->number_of_walls].x1 = 1; map->walls[map->number_of_walls].y1 = 0; map->walls[map->number_of_walls].x2 = 1; map->walls[map->number_of_walls].y2 = 1;
//        map->number_of_walls++;
//        vertex[1][1] = 1;
//    }
    int i, j;
    while (exist_0_vertex()) {
        for (i = 0; i < 12; i++) {
            for (j = 0; j < 7; ++j) {
                while (1) {
                    int number_of_ways = 0;
                    int is_ok_to_go[4] = {};
                    int warn_to_go[4] = {};
                    for (int k = 0; k < 4; ++k) {
                        if (j == 0) {
                            warn_to_go[1] = 1;
                        }
                        if (j == 6) {
                            warn_to_go[0] = 1;
                        }
                        if (i == 0) {
                            warn_to_go[3] = 1;
                        }
                        if (i == 11) {
                            warn_to_go[2] = 1;
                        }
                        if (warn_to_go[k] == 0 && vertex[i + dx[k]][j + dy[k]] == 0) {
                            number_of_ways++;
                            is_ok_to_go[k] = 1;
                        }
                    }
                    if (number_of_ways == 0) {
                        break;
                    }
                    int k = rand() % 4;
                    while (is_ok_to_go[k] == 0) {
                        k = rand() % 4;
                    }
                    if (rand() % 3) {
                        map->walls[map->number_of_walls].x1 = i;
                        map->walls[map->number_of_walls].y1 = j;
                        map->walls[map->number_of_walls].x2 = i + dx[k];
                        map->walls[map->number_of_walls].y2 = j + dy[k];
                        map->walls[map->number_of_walls].vertical = is_wall_vertical(i, i + dx[k]);
                        map->number_of_walls++;
                        vertex[i][j] = 1;
                        vertex[i + dx[k]][j + dy[k]] = 1;
                    }
                    i += dx[k];
                    j += dy[k];
                }
            }
        }
    }

    //convert wall numbers to big one
    for (int k = 0; k < map->number_of_walls; ++k) {
        map->walls[k].x1 *= 100;
        map->walls[k].y1 *= 100;
        map->walls[k].x2 *= 100;
        map->walls[k].y2 *= 100;
        map->walls[k].x1 += 20;
        map->walls[k].y1 += 20;
        map->walls[k].x2 += 20;
        map->walls[k].y2 += 20;
    }
}

int pow_2(int a) {
    return a * a;
}

int collid_to_vertex(int temp_x, int temp_y) {
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 7; ++j) {
            if (pow_2(temp_x - (i * 100 + 20)) + pow_2(temp_y - (j * 100 + 20)) < pow_2(tank_radius)) {
                return 1;
            }
        }
    }
    return 0;
}

int collid_to_edge(Map *map, int temp_x, int temp_y) {
    for (int i = 0; i < map->number_of_walls; ++i) {
        if (map->walls[i].vertical) {
            if ((map->walls[i].y1 <= temp_y && temp_y <= map->walls[i].y2) || (map->walls[i].y2 <= temp_y && temp_y <= map->walls[i].y1)) {
                if (pow_2(temp_x - map->walls[i].x1) < pow_2(tank_radius)) {
                    return 1;
                }
            }
        } else {
            if ((map->walls[i].x1 <= temp_x && temp_x <= map->walls[i].x2) || (map->walls[i].x2 <= temp_x && temp_x <= map->walls[i].x1)) {
                if (pow_2(temp_y - map->walls[i].y1) < pow_2(tank_radius)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int tank_movement_collid(Map *map, int temp_x, int temp_y) {
    if (collid_to_vertex(temp_x, temp_y)) {
        return 1;
    }
    if (collid_to_edge(map, temp_x, temp_y)) {
        return 1;
    }
    return 0;
}

void random_line_place(Map *map) {
    srand((unsigned int)(time(NULL)));
    for (int i = 0; i < 30; ++i) {
        Line *line = &(map->lines[i]);
        line->x1 = rand() % x_max;
        line->y1 = rand() % y_max;
        line->x2 = rand() % x_max;
        line->y2 = rand() % y_max;
        for (int j = 0; j < 3; ++j) {
            line->color[j] = (rand() % 5) * 45;
        }
    }
}

void add_score(Map *map) {
    for (int i = 0; i < 3; ++i) {
        if (map->tank[i].is_alive) {
            map->tank[i].score++;
        }
        if (map->tank[i].score == win_score) {
            map->game_finished = 1;
            map->game_pause = 1;
            map->first_menu = 1;
            time_passed_during_game = map->frames / FPS;
            random_line_place(map);
        }
    }
}

int is_two_dead(Map *map) {
    int deads = 0;
    for (int i = 0; i < 3; ++i) {
        if (map->tank[i].is_alive == 0) {
            deads++;
        }
    }
    if (deads >= 2) {
        return 1;
    }
    return 0;
}

void bullet_collid_tank(Bullet *bullet, Map *map) {
    for (int i = 0; i < 3; ++i) {
        Tank *tank = &(map->tank[i]);
        if (tank->is_alive) {
            if (pow_2(bullet->x - tank->x) + pow_2(bullet->y - tank->y) < pow_2(tank_radius)) {
                bullet->is_fired = 0;
                kill_tank(map, tank);
                return;
            }
        }
    }
}

void kill_tank(Map *map, Tank *tank) {
    tank->is_alive = 0;
    add_score(map);
    if (is_two_dead(map)) {
        generate_map(map);
        zero_key_pressed(map);
        tanks_rand_place(map);
    }
}

void power_in_myplace(Tank *tank, Powerup *powerup) {
    if (pow_2(tank->x - powerup->x) + pow_2(tank->y - powerup->y) < pow_2(tank_radius + 10)) {
        powerup->is_on = 0;
        if (powerup->type < 3) {
            tank->powered_up = 1;
        }
        switch (powerup->type) {
            case 0:
                tank->frag_section = 1;
                break;
            case 1:
                tank->have_mine = 1;
                break;
        }
    }
}

void get_powerup(Map *map, Tank *tank) {
    if (tank->is_alive == 0) {
        return;
    }
    for (int i = 0; i < 5; ++i) {
        Powerup *p_power = &(map->powerup[i]);
        if (p_power->is_on) {
            if (p_power->type < 3) {
                if (tank->powered_up == 0) {
                    power_in_myplace(tank, p_power);
                }
            } else {
                power_in_myplace(tank, p_power);
            }
        }
    }
}

int in_Wall(Bullet *bullet, Map *map, Wall **pwall) {
    for (int i = 0; i < map->number_of_walls; ++i) {
        Wall *wally = &(map->walls[i]);
        if (wally->vertical) {
            if ( (wally->y1 <= bullet->y && bullet->y <= wally->y2) || (wally->y2 <= bullet->y && bullet->y <= wally->y1) ) {
                if (abs(bullet->x - wally->x1) <= 5) {
                    *pwall = wally;
                    return 1;
                }
            }
        } else {
            if ( (wally->x1 <= bullet->x && bullet->x <= wally->x2) || (wally->x2 <= bullet->x && bullet->x <= wally->x1) ) {
                if (abs(bullet->y - wally->y1) <= 5) {
                    *pwall = wally;
                    return 1;
                }
            }
        }
    }
    return 0;
}

void bullet_collid_wall(Bullet *bullet, Map *map) {
    Wall *pwall;
    if (bullet->in_wall == 0) {
        if (in_Wall(bullet, map, &pwall)) {
            bullet->in_wall = 1;
            if (pwall->vertical) {
                bullet->angle = pi - bullet->angle;
            } else {
                bullet->angle = -bullet->angle;
            }
        }
    } else {
        if (in_Wall(bullet, map, &pwall) == 0) {
            bullet->in_wall = 0;
        }
    }
}

void set_a_powerup(Map *map) {
    srand((unsigned int)(time(NULL)));
    int i;
    for (i = 0; i < 5; ++i) {
        if (map->powerup[i].is_on == 0) {
            map->powerup[i].is_on = 1;
            break;
        }
    }
    if (i == 5) {
        return;
    }
    map->powerup[i].type = rand() % 2;//now just Frag Bomb and Mine
    map->powerup[i].use_by = 0;//now for ALL
    powerup_rand_place(&(map->powerup[i]));
}

void mine_features_controlling(Map *map) {
    for (int i = 0; i < 15; ++i) {
        Mine *purpose = &(map->mine[i]);
        if (purpose->is_planted) {
            purpose->frame_from_born++;
        }
        if (purpose->is_in_range) {
            purpose->frame_from_in_range++;
            if (purpose->frame_from_in_range > 9) {
                purpose->is_planted = 0;
                purpose->frame_from_born = 0;
                purpose->is_in_range = 0;
                purpose->frame_from_in_range = 0;
                //destroy around area
                for (int j = 0; j < 3; ++j) {
                    Tank *p_tank = &(map->tank[j]);
                    if (p_tank->is_alive) {
                        tank_on_mine_range(map, p_tank, purpose, 1);
                    }
                }
            }
        }
    }
}

void tank_on_mine_range(Map *map, Tank *tank, Mine *mine, int to_destroy) {
    if (pow_2(tank->x - mine->x) + pow_2(tank->y - mine->y) < pow_2(2 * tank_radius)) {
        if (mine->is_in_range == 0) {
            mine->frame_from_in_range = 0;
        }
        mine->is_in_range = 1;
        if (to_destroy) {
            kill_tank(map, tank);
        }
    }
}