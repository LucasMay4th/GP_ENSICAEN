#include <stdlib.h>
#include "priority_queue.h"

PriorityQueue* create_priority_queue(int capacity) {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pq->nodes = (PQNode*)malloc(capacity * sizeof(PQNode));
    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

void free_priority_queue(PriorityQueue* pq) {
    if (pq != NULL) {
        free(pq->nodes);
        free(pq);
    }
}

static void swap_nodes(PQNode* a, PQNode* b) {
    PQNode temp = *a;
    *a = *b;
    *b = temp;
}

void push_pq(PriorityQueue* pq, int cell_index, int cost) {
    int current_idx, parent_idx;
    
    /* Sécurité : si la file est pleine, on pourrait réallouer, mais 
       ici on aura au max (width * height) éléments */
    if (pq->size >= pq->capacity) return;

    /* On ajoute à la fin */
    current_idx = pq->size;
    pq->nodes[current_idx].cell_index = cell_index;
    pq->nodes[current_idx].cost = cost;
    pq->size++;

    /* Heapify Up (remonter l'élément s'il est plus petit que son parent) */
    while (current_idx > 0) {
        parent_idx = (current_idx - 1) / 2;
        if (pq->nodes[current_idx].cost >= pq->nodes[parent_idx].cost) {
            break;
        }
        swap_nodes(&pq->nodes[current_idx], &pq->nodes[parent_idx]);
        current_idx = parent_idx;
    }
}

PQNode pop_pq(PriorityQueue* pq) {
    PQNode min_node = pq->nodes[0];
    int current_idx = 0;
    int left_child, right_child, smallest;

    /* Remplacer la racine par le dernier élément */
    pq->size--;
    pq->nodes[0] = pq->nodes[pq->size];

    /* Heapify Down (descendre l'élément s'il est plus grand que ses enfants) */
    while (1) {
        left_child = 2 * current_idx + 1;
        right_child = 2 * current_idx + 2;
        smallest = current_idx;

        if (left_child < pq->size && pq->nodes[left_child].cost < pq->nodes[smallest].cost) {
            smallest = left_child;
        }
        if (right_child < pq->size && pq->nodes[right_child].cost < pq->nodes[smallest].cost) {
            smallest = right_child;
        }

        if (smallest == current_idx) {
            break;
        }

        swap_nodes(&pq->nodes[current_idx], &pq->nodes[smallest]);
        current_idx = smallest;
    }

    return min_node;
}

int is_pq_empty(PriorityQueue* pq) {
    return pq->size == 0;
}