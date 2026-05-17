#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "../list/list.h"

/**
 * @brief Creates a new, empty queue.
 *
 * @return A pointer to the newly created queue.
 */
 Queue* createQueue() {
    Queue* q;
    q =(Queue*)malloc(sizeof(Queue));
    q->front=NULL;
    q->rear=NULL;
    return q;
}


/**
 * @brief Checks whether the queue is empty.
 *
 * @param q The queue to check.
 * @return 1 if the queue is empty, 0 otherwise.
 */
int isQueueEmpty(Queue q) {
    if (q.front==NULL){
        return 1;
    }
    return 0;
}


/**
 * @brief Prints all the elements in the queue, from front to rear.
 *
 * @param q The queue to print.
 */
void queuePrint(Queue q) {
    Cell* curr;
    if (q.front == NULL) {
        printf("[]");
        return;
    }
    printf("[");
    curr =q.front;
    while(curr!=q.rear){
        printf("%d,",curr->value);
        curr=curr->nextCell;
    }
    printf("%d",curr->value);
    printf("]");
    return;
}


/**
 * @brief Adds an element to the rear of the queue.
 *
 * @param q A pointer to the queue to add the element to.
 * @param data The data to be added to the queue.
 */
void enqueue(Queue* q, int data) {
    Cell* new;
    new = (Cell*)malloc(sizeof(Cell));
    new->nextCell=NULL;
    new->value=data;
     if (q->rear == NULL) {         
        q->front = new;
        q->rear = new;
    } else {
        q->rear->nextCell=new;
        q->rear=new;
    }
    return;
}

/**
 * @brief Removes and returns the front element of the queue.
 *
 * @param q A pointer to the queue to remove the front element from.
 * @return The value stored in the front element of the queue.
 */
int dequeue(Queue* q) {
    int elem;
    Cell* first;;
    if (q->front == NULL) {
        return -1;
    }
    elem=q->front->value;
    first=q->front;
    q->front=first->nextCell;
    if (q->front == NULL) {
        q->rear = NULL;
    }


    free(first);
    return elem;
}


/**
 * @brief Returns the value of the front node but doesn't modify the queue
 *
 * @param q The queue to get the front value.
 * @return The value of the front node.
 */
int queueGetFrontValue(Queue q) {

    return q.front->value;
}
