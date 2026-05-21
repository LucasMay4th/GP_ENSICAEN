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
 * @file ai_decision.h
 *
 * En-tête du moteur de décision. Contient la fonction d'exploration DFS 
 * pour trouver le meilleur vecteur d'accélération.
 */

#ifndef AI_DECISION_H
#define AI_DECISION_H

#include "types.h"

int simulate_best_move_dfs(Map* map, CarState current_state, int depth, int is_first_turn, int* best_ax, int* best_ay);

#endif