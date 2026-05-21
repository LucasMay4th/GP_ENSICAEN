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
 * @file physics.c
 *
 * Implémentation des règles du jeu : validation des mouvements intermédiaires 
 * pour interdire la coupe, calcul de pénalités de sable et consommation d'essence.
 */

#include <math.h>
#include "../Include/physics.h"
#include "../../follow_line/follow_line.h" 

int calculate_gas_consumption(int accX, int accY, int speedX, int speedY, int is_in_sand) {
    int gas = accX * accX + accY * accY;
    gas += (int)floor(sqrt(1.5 * (speedX * speedX + speedY * speedY)));
    if (is_in_sand) {
        gas += 1;
    }
    return gas; 
}

int validate_move_trajectory(Map* map, Position start, Position end, int* crossed_finish, int* crossed_sand, int check_opponents) {
    InfoLine infoLine;
    Pos2Dint p; 
    char t;
    *crossed_finish = 0; 
    *crossed_sand = 0;
    
    initLine(start.x, start.y, end.x, end.y, &infoLine);
    
    while(nextPoint(&infoLine, &p, 1) > 0) {
        if(p.x == start.x && p.y == start.y) continue;
         
        if(p.x < 0 || p.x >= map->width || p.y < 0 || p.y >= map->height) return 0; 
        
        t = map->grid[p.y][p.x].type;
        if (t != '#' && t != '~' && t != '=' && !(t >= '1' && t <= '3')) return 0; 
        if (check_opponents && map->grid[p.y][p.x].has_opponent == 1) return 0; 
        
        if (t == '=') *crossed_finish = 1;
        if (t == '~') *crossed_sand = 1;
    }
    return 1; 
}