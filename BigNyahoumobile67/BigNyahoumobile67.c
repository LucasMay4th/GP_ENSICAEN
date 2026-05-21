/*
 * ENSICAEN
 * 6 Boulevard Marechal Juin
 * F-14050 Caen Cedex
 *
 * This file is owned by ENSICAEN.
 * No portion of this document may be reproduced, copied
 * or revised without written permission of the authors.
 */

/**
 *
 * @author Lucas Brunet <lucas.brunet251@ecole.ensicaen.fr>
 * @author Axel Favreau-Perfetti <axel.favreau-perfetti@ecole.ensicaen.fr>
 * @version 0.1.1- 2025/10/03
 * Gemini Pro 3.1 used for write pseudo-code 
 */

/**
 * @file BigNyahoumobile67.c
 *
 * Point d'entrée principal du pilote de Formule 1. 
 * Gère la boucle de communication sur les entrées/sorties standards 
 * avec le gestionnaire de course (lecture de la carte, des positions, et envoi des commandes).
 */

#include <stdio.h>
#include <stdlib.h>
#include "Include/types.h"
#include "Include/map_manager.h"
#include "Include/physics.h"
#include "Include/ai_decision.h"

void sync_car_physics_with_server(CarState* me, Position previous_pos, int expected_vx, int expected_vy, int* consecutive_no_move) {
    int expected_x = previous_pos.x + expected_vx;
    int expected_y = previous_pos.y + expected_vy;

    if (me->pos.x == expected_x && me->pos.y == expected_y) {
        me->vx = expected_vx;
        me->vy = expected_vy;
    } else {
        me->vx = 0;
        me->vy = 0;
    }

    if (me->pos.x == previous_pos.x && me->pos.y == previous_pos.y) {
        (*consecutive_no_move)++;
    } else {
        *consecutive_no_move = 0;
    }
}

int main(void) {
    int width, height, gasLevel, row, col;
    char line_buffer[MAX_LINE_LENGTH];
    Map* race_map = NULL;
    CarState me = { {0,0}, 0, 0, 0, MAX_BOOSTS };
    CarState opp1 = { {0,0}, 0, 0, 0, 0 };
    CarState opp2 = { {0,0}, 0, 0, 0, 0 };
    
    int round = 0;
    int is_first_round = 1;
    Position previous_pos = {0,0};
    int expected_vx = 0, expected_vy = 0;
    int consecutive_no_move = 0;
    int best_accX = 0, best_accY = 0;
    
    if (fgets(line_buffer, MAX_LINE_LENGTH, stdin) == NULL) return EXIT_FAILURE;
    sscanf(line_buffer, "%d %d %d", &width, &height, &gasLevel);
    me.gas = gasLevel;
    
    race_map = allocate_race_map(width, height);
    
    for (row = 0; row < height; ++row) {
        if (fgets(line_buffer, MAX_LINE_LENGTH, stdin) != NULL) {
            for (col = 0; col < width; ++col) {
                race_map->grid[row][col].type = line_buffer[col];
                race_map->grid[row][col].has_opponent = 0;
            }
        }
    }
    
    compute_dijkstra_distances_to_finish(race_map); 

    while (!feof(stdin)) {
        if (fgets(line_buffer, MAX_LINE_LENGTH, stdin) == NULL) break;
        
        sscanf(line_buffer, "%d %d %d %d %d %d",
               &me.pos.x, &me.pos.y, 
               &opp1.pos.x, &opp1.pos.y, 
               &opp2.pos.x, &opp2.pos.y);
        
        if (!is_first_round) {
            sync_car_physics_with_server(&me, previous_pos, expected_vx, expected_vy, &consecutive_no_move);
        } else {
            me.vx = 0;
            me.vy = 0;
            is_first_round = 0;
        }
        
        if (consecutive_no_move >= 20) {
            fprintf(stderr, "=== CRASH : Voiture immobile depuis 20 tours. Fin du pilote. ===\n");
            break;
        }
        
        place_opponents_on_map(race_map, opp1, opp2);
        
        /* Profondeur ajustée pour la gestion des boosts (25 possibilités par branche) */
        simulate_best_move_dfs(race_map, me, 4, 1, &best_accX, &best_accY);
        
        remove_opponents_from_map(race_map);
        
        fprintf(stdout, "%d %d\n", best_accX, best_accY);
        fflush(stdout); 
        
        previous_pos = me.pos;
        expected_vx = me.vx + best_accX;
        expected_vy = me.vy + best_accY;
        round++;
    }
    
    free_race_map(race_map);
    return EXIT_SUCCESS;
}