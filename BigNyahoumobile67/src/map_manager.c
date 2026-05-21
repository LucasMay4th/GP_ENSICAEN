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
 * @file map_manager.c
 *
 * Implémentation des fonctions de gestion de la carte et du parcours de graphe (Dijkstra)
 * pour évaluer les coûts de déplacement jusqu'à la ligne d'arrivée.
 */

#include <stdlib.h>
#include "../Include/map_manager.h"
#include "../structure_de_base/priority_queue/priority_queue.h"

Map* allocate_race_map(int width, int height) {
    int i;
    Map* map = (Map*)malloc(sizeof(Map));
    map->width = width;
    map->height = height;
    map->grid = (MapCell**)malloc(height * sizeof(MapCell*));
    for(i = 0; i < height; i++){
        map->grid[i] = (MapCell*)malloc(width * sizeof(MapCell));
    }
    return map;
}

void free_race_map(Map* map) {
    int i;
    if (map != NULL) {
        for(i = 0; i < map->height; i++){
            free(map->grid[i]);
        }
        free(map->grid);
        free(map);
    }
}

void compute_dijkstra_distances_to_finish(Map* map) {
    int dx[8] = {-1,  1,  0,  0, -1, -1,  1,  1};
    int dy[8] = { 0,  0, -1,  1, -1,  1, -1,  1};
    int move_costs[8] = {10, 10, 10, 10, 14, 14, 14, 14}; 
    int i, j, dir, x, y, nx, ny, cell_index, new_cost;
    char neighbor_type;
    PQNode current_node;
    
    PriorityQueue* pq = create_priority_queue(map->width * map->height);
    
    for(i = 0; i < map->height; i++) {
        for(j = 0; j < map->width; j++) {
            if(map->grid[i][j].type == '=') {
                map->grid[i][j].dist_to_goal = 0;
                cell_index = i * map->width + j; 
                push_pq(pq, cell_index, 0);
            } else {
                map->grid[i][j].dist_to_goal = INF;
            }
        }
    }
    
    while(!is_pq_empty(pq)) {
        current_node = pop_pq(pq);
        y = current_node.cell_index / map->width; 
        x = current_node.cell_index % map->width; 
        
        if (current_node.cost > map->grid[y][x].dist_to_goal) continue;
        
        for (dir = 0; dir < 8; dir++) {
            nx = x + dx[dir];
            ny = y + dy[dir];
            
            if (nx >= 0 && nx < map->width && ny >= 0 && ny < map->height) {
                neighbor_type = map->grid[ny][nx].type;
                if (neighbor_type == '#' || neighbor_type == '~' || neighbor_type == '=' || (neighbor_type >= '1' && neighbor_type <= '3')) {
                    new_cost = map->grid[y][x].dist_to_goal + move_costs[dir];
                    if (neighbor_type == '~') new_cost += 50; 
                    
                    if (new_cost < map->grid[ny][nx].dist_to_goal) {
                        map->grid[ny][nx].dist_to_goal = new_cost;
                        push_pq(pq, ny * map->width + nx, new_cost);
                    }
                }
            }
        }
    }
    free_priority_queue(pq);
}

void place_opponents_on_map(Map* map, CarState opponent1, CarState opponent2) {
    if (opponent1.pos.x >= 0 && opponent1.pos.x < map->width && opponent1.pos.y >= 0 && opponent1.pos.y < map->height) {
        map->grid[opponent1.pos.y][opponent1.pos.x].has_opponent = 1;
    }
    if (opponent2.pos.x >= 0 && opponent2.pos.x < map->width && opponent2.pos.y >= 0 && opponent2.pos.y < map->height) {
        map->grid[opponent2.pos.y][opponent2.pos.x].has_opponent = 1;
    }
}

void remove_opponents_from_map(Map* map) {
    int i, j;
    for(i = 0; i < map->height; i++) {
        for(j = 0; j < map->width; j++) {
            map->grid[i][j].has_opponent = 0;
        }
    }
}