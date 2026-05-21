#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

/* Un noeud dans la file de priorité */
typedef struct {
    int cell_index; /* L'index 1D de la case (y * width + x) */
    int cost;       /* La distance (priorité) de cette case */
} PQNode;

/* La structure de la file de priorité (Min-Heap) */
typedef struct {
    PQNode* nodes;
    int size;
    int capacity;
} PriorityQueue;

/* Fonctions de gestion de la file de priorité */
PriorityQueue* create_priority_queue(int capacity);
void free_priority_queue(PriorityQueue* pq);
void push_pq(PriorityQueue* pq, int cell_index, int cost);
PQNode pop_pq(PriorityQueue* pq);
int is_pq_empty(PriorityQueue* pq);

#endif