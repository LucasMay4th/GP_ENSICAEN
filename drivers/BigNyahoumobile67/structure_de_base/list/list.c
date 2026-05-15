/**
 * @file list.c
 * @brief Implementation file for a linked list
 *
 * This file contains the implementation of the linked list data structure
 * and its associated functions.
 *
 * @author Loïck Lhote
 * @date February 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/**
 * @brief Creates a new linked list
 *
 * @return A pointer to the newly created linked list
 */
List newList() {
    return NULL;
}

/**
 * @brief Frees the memory used by a linked list
 *
 * @param L Pointer to the linked list to be freed
 */
void freeList(List L) {
    if(L==NULL){
        return;
    }
    Cell *curr;   
    while(L!= NULL) {
        curr = L;
        L= L->nextCell;
        free(curr);
    }
    
    return;
}

/**
 * @brief Prints the contents of a linked list
 *
 * @param L Pointer to the linked list to be printed
 * @param type If type==0, prints only the values, otherwise print the couples (key,vlaue)
 *
 * Prints the linked list in the following way if k!=0:
 * [(key 1, value 1),(key 2, value 2), … ,(key k, value k)]
 * Prints the linked list in the following way if k==0:
 * [value 1,value 2,…,value k]
 */

void printList(List L, int type) {
    if(L==NULL){
        printf("[]");
        return;
    }   
    Cell *curr=L;

    printf("[");
    switch (type)
    {
    case 0:
        while(curr != NULL){

            printf("%d",curr->value);
            curr=curr->nextCell;
            if (curr!= NULL){
                printf(",");
            }
        }
        break;
    
    default:
        while(curr != NULL){

            if (curr->key == NULL) {
                printf("(NULL,%d)", curr->value);
            } else {
                printf("(%s,%d)", curr->key, curr->value);
            }
            curr=curr->nextCell;
            if (curr!= NULL){
                printf(",");
            }
        };

        break;
    }
    printf("]");
    return;
}
/**
 * @brief Finds a key in a linked list
 *
 * @param L Pointer to the linked list to search in
 * @param key Key to search for
 *
 * @return A pointer to the first cell containing the key, or NULL if the key is not found
 */
Cell* findKeyInList(List L, string key) {
    if(L==NULL) {
        return NULL;
    } 
    Cell *curr =L->nextCell;
    
    if(key == NULL){
            while(curr!=NULL){
        
            if (curr->key == NULL){
                return curr;
            }
            curr= curr->nextCell;

            }
    }
    while(curr!=NULL){
      
        if (curr->key != NULL && strcmp(curr->key, key) == 0){
            return curr;
        }
        curr= curr->nextCell;

    }
    
    return NULL;
}

/**
 * @brief Deletes a key from a linked list
 *
 * @param L Pointer to the linked list to search in
 * @param key Key to delete
 *
 * @return A pointer to the modified linked list
 * Only the first occurrence of the key is deleted.
 */
List delKeyInList(List L, string key) {
   
    if(L==NULL) { 
        return NULL;
    } 

    Cell *curr=L;
    Cell *prev=L;
    switch (key == NULL)
    {
    case 0:
        if (L->key != NULL ){
            if(strcmp(L->key,key)==0){
                L=curr->nextCell;
                free(curr);
                return L;
            } 
        }
        curr=curr->nextCell;
        int flag =0;
        while (flag !=1){
            if (curr->key!= NULL){
                if (strcmp(curr->key,key)==0){
                    prev->nextCell = curr->nextCell;
                    free(curr);
                    return L;
                }
            }
            if (curr->nextCell ==NULL){
                flag =1;
            }
            prev=curr;    
            curr=curr->nextCell;
        }
        return L;

    
    default:
        if (L->key == NULL ){
            L=curr->nextCell;
            free(curr);
            return L;
        }
        curr=curr->nextCell;
        
        while (curr->key!=NULL && curr->nextCell !=NULL){
            prev=curr;    
            curr=curr->nextCell;
        }
        if (curr->key ==NULL){
            prev->nextCell = curr->nextCell;
            free(curr);
            return L;
        }else{
            return L;
        }
        break;
    }
}
 
/**
 * @brief Adds a key-value pair to a linked list
 *
 * @param L Pointer to the linked list to add to
 * @param key Key to add
 * @param value Value to add
 *
 * @return A pointer to the modified linked list
 *
 * The key-value pair is added at the beginning of the list
 */

 List addKeyValueInList(List L, string key, int value) {
    Cell* new= (Cell*)malloc(sizeof(struct cell));
    new->key=key;
    new->value=value;
    new->nextCell=L;
    return new;
}


