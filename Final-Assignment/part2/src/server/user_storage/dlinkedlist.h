#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "user_storage.h"

#define MAXSIZE 256

/* Definition of a node of the data structure */
typedef struct Node  {
    void* data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct dll {
  Node* head;
} dll;

unsigned int next_message_id;

dll* createList(void);

/* Deletes all the list and frees the memory */
int destroyList(dll* list);

void printList(dll* list);

/* Method to create a new node */
Node* getNewNode(void* data);

/* Method to insert a new node in the list. This node will be added at the end */
int insert_user(dll* list, Node* newNode);

/* Method to insert a new node in the list. This node will be added at the end */
int insert_msg(dll* list, Node* newNode);

/* Deletes the node with the provided key */
int delete_user(dll* list, char* username);

int delete_msg(dll* list, unsigned int id);

/* Returns the node with the provided key associated */
Node* search_user(dll* list, char* username);

/* Returns the node with the provided key associated */
Node* search_msg(dll* list, unsigned int id);

int modify_user(dll* list, Node* newNode);

int modify_msg(dll* list, Node* newNode);

/* Returns the number of nodes currently stored in the list */
unsigned int getCardinality(dll* list);

int get_total_messages_by_user(dll* list, char* username);

#endif /* DLINKEDLIST */
