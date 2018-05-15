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

unsigned int next_message_id;

/* Deletes all the list and frees the memory */
int destroyList(Node* head);

/* Method to create a new node */
Node* getNewNode(void* data);

/* Method to insert a new node in the list. This node will be added at the end */
int insert_user(Node* head, Node* newNode);

/* Method to insert a new node in the list. This node will be added at the end */
int insert_msg(Node* head, Node* newNode);

/* Deletes the node with the provided key */
int delete_user(Node* head, char* username);

int delete_msg(Node* head, unsigned int id);

/* Returns the node with the provided key associated */
Node* search_user(Node* head, char* username);

/* Returns the node with the provided key associated */
Node* search_msg(Node* head, unsigned int id);

int modify_user(Node* head, Node* newNode);

int modify_msg(Node* head, Node* newNode);

/* Returns the number of nodes currently stored in the list */
unsigned int getCardinality(Node* head);

int get_total_messages_by_user(Node* head, char* username);

#endif /* DLINKEDLIST */
