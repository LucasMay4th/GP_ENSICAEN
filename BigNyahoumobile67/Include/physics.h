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
 * @file physics.h
 *
 * En-tête des règles physiques de la course : calcul de la consommation d'essence 
 * et validation des trajectoires de Bresenham.
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#include "types.h"

int calculate_gas_consumption(int accX, int accY, int speedX, int speedY, int is_in_sand);
int validate_move_trajectory(Map* map, Position start, Position end, int* crossed_finish, int* crossed_sand, int check_opponents);

#endif