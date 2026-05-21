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
 * @file ai_decision.c
 *
 * Implémentation du moteur de recherche récursif (DFS).
 * Évalue les trajectoires futures (incluant les boosts) pour retourner 
 * la meilleure décision à prendre pour le tour actuel.
 */

#include <stdlib.h>
#include "../Include/ai_decision.h"
#include "../Include/physics.h"

int simulate_best_move_dfs(Map* map, CarState current_state, int depth, int is_first_turn, int* best_ax, int* best_ay) {
    int meilleur_score = -INF;
    int ax, ay, score, nouvelle_vx, nouvelle_vy, norme_carre, crossed_finish, crossed_sand, in_sand_start;
    int eval_score, v_carre, dist, is_boosting;
    CarState new_state;
    Position new_pos;
    
    if(current_state.gas <= 0) {
        dist = map->grid[current_state.pos.y][current_state.pos.x].dist_to_goal;
        if (dist >= INF) return -INF + 1000;
        return -(dist * 100) - 50000;
    }
    
    if(depth == 0) {
        eval_score = 0;
        v_carre = (current_state.vx * current_state.vx) + (current_state.vy * current_state.vy);
        dist = map->grid[current_state.pos.y][current_state.pos.x].dist_to_goal;
        
        if (dist >= INF) return -INF + 1000;
        
        eval_score -= (dist * 100);   
        eval_score += (current_state.gas * 600);  
        eval_score -= (v_carre * 150);
        eval_score += (current_state.boosts_left * 50);
        
        return eval_score;
    }
    
    in_sand_start = (map->grid[current_state.pos.y][current_state.pos.x].type == '~') ? 1 : 0;
    
    for (ax = -2; ax <= 2; ax++) {
        for (ay = -2; ay <= 2; ay++) {
            
            is_boosting = (abs(ax) == 2 || abs(ay) == 2);
            if (is_boosting && current_state.boosts_left <= 0) continue;
            
            nouvelle_vx = current_state.vx + ax;
            nouvelle_vy = current_state.vy + ay;
            norme_carre = (nouvelle_vx * nouvelle_vx) + (nouvelle_vy * nouvelle_vy);
            
            if (norme_carre > 25) continue;
            if (in_sand_start && norme_carre > 1) continue;
            
            new_pos.x = current_state.pos.x + nouvelle_vx;
            new_pos.y = current_state.pos.y + nouvelle_vy;
            
            if (new_pos.x < 0 || new_pos.x >= map->width || new_pos.y < 0 || new_pos.y >= map->height) continue;
            
            crossed_finish = 0;
            crossed_sand = 0;
            
            if(validate_move_trajectory(map, current_state.pos, new_pos, &crossed_finish, &crossed_sand, is_first_turn) == 0) continue;
            if (crossed_sand == 1 && norme_carre > 1) continue;
            
            if (crossed_finish == 1) {
                score = 1000000 + (current_state.gas * 10) + (depth * 10000); 
            } else {
                new_state.pos = new_pos;
                new_state.vx = nouvelle_vx;
                new_state.vy = nouvelle_vy;
                
                if (crossed_sand) {
                    new_state.vx = (new_state.vx > 0) ? 1 : ((new_state.vx < 0) ? -1 : 0);
                    new_state.vy = (new_state.vy > 0) ? 1 : ((new_state.vy < 0) ? -1 : 0);
                }
                
                new_state.gas = current_state.gas - calculate_gas_consumption(ax, ay, current_state.vx, current_state.vy, crossed_sand);
                new_state.boosts_left = current_state.boosts_left - (is_boosting ? 1 : 0);
                
                score = simulate_best_move_dfs(map, new_state, depth - 1, 0, NULL, NULL);
            }
            
            if(meilleur_score == -INF || score > meilleur_score) {
                meilleur_score = score;
                if(is_first_turn == 1 && best_ax != NULL && best_ay != NULL) {
                    *best_ax = ax;
                    *best_ay = ay;
                }
            }
        }
    }
    
    if (meilleur_score == -INF) {
        return -1000000 - (depth * 1000);
    }
    
    return meilleur_score;
}