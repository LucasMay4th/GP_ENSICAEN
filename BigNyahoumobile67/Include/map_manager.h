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
 *
 */

/**
 * @file map_manager.h
 *
 * En-tête pour la gestion de la carte : allocation mémoire, placement des adversaires 
 * et calcul des plus courts chemins (Dijkstra).
 */

#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include "types.h"

Map* allocate_race_map(int width, int height);
void free_race_map(Map* map);
void compute_dijkstra_distances_to_finish(Map* map);
void place_opponents_on_map(Map* map, CarState opponent1, CarState opponent2);
void remove_opponents_from_map(Map* map);

#endif