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
 * @file types.h
 *
 * Définition des structures de données partagées (Carte, Etat de la voiture, Cellule).
 */

#ifndef TYPES_H
#define TYPES_H

#define MAX_LINE_LENGTH 1024
#define MAX_BOOSTS 5
#define INF 1000000000

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position pos;
    int vx;
    int vy;
    int gas;
    int boosts_left;
} CarState;

typedef struct {
    char type;         
    int dist_to_goal;  
    int has_opponent;  
} MapCell;

typedef struct {
    MapCell** grid;
    int width;
    int height;
} Map;

#endif