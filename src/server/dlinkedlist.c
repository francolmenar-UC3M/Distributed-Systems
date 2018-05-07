#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user.h"

#define MAXSIZE 256

/* Definition of a node of the data structure */
typedef struct Node  {
    struct user* data;
    struct Node* next;
    struct Node* prev;
} Node;

/* Head of the Doubly Linked List*/
Node* head;

/* Deletes all the list and frees the memory */
int destroyList() {
    if(head == NULL) return 0;

    Node* node = head;
    Node* temp;
    while(node != NULL) {
        temp = node;
        node = node->next;
        free(temp);
    }

    head = NULL;

    return 0;
}

/* Method to create a new node */
Node* getNewNode(struct user* data) {
    Node* newNode = (Node*)malloc(sizeof(Node));

    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}

/* Method to print in stdout the current nodes and their values (debugging) */
void printList() {
	Node* temp = head;
  int counter = 0;
	while(temp != NULL) {
		printf("NODE %d: %s, status: %d, %s:%d\n", counter, temp->data->username, temp->data->status, inet_ntoa(*(temp->data->ip_address)), temp->data->port);
		temp = temp->next;
    counter++;
	}
}

/* Method to insert a new node in the list. This node will be added at the end */
int insert(Node* newNode) {
    Node* temp = head;

    if(head == NULL) {
        head = newNode;
        return 0;
    }

    while(temp != NULL) {
        if(strcmp(temp->data->username, newNode->data->username) == 0) {
            /* Key already exists */
            printList();
            return -1;
        }

        if(temp->next != NULL) {
            temp = temp->next;
        } else {
            break;
        }
    }

    temp->next = newNode;
    newNode->prev = temp;
    return 0;
}

/* Deletes the node with the provided key */
int delete(char* username) {
    Node* temp = head;

    if(head == NULL) {
        return -1;
    }

    while(temp != NULL) {
        if(strcmp(temp->data->username, username) == 0) {
            if(temp == head) {
                head = temp->next;
                free(temp);
                return 0;
            }

            Node* prev = temp->prev;
            Node* next = temp->next;

            prev->next = temp->next;
            next->prev = temp->prev;

            free(temp);
            return 0;
        }
        temp = temp->next;
    }

    return -1;
}

/* Returns the node with the provided key associated */
Node* search(char* username) {
    Node* temp = head;

    if(head == NULL) {
        return NULL;
    }

    while(temp != NULL) {
      if(strcmp(temp->data->username, username) == 0) {
            return temp;
        }
	      temp = temp->next;
    }

    return NULL;
}


int modify(Node* newNode) {
    Node* temp = head;

    if(head == NULL) {
        /* Empty list */
        return -1;
    }

    while(temp != NULL) {
      if(strcmp(temp->data->username, newNode->data->username) == 0) {
            /* Key found */
            temp->data = newNode->data;
            printList();
            return 0;
        }

        if(temp->next != NULL) {
            temp = temp->next;
        } else {
            break;
        }
    }
    /* Key not found */
    return -1;
}

/* Returns the number of nodes currently stored in the list */
int getCardinality() {
    Node* temp = head;
    int count = 0;

    if(head == NULL) {
      /* Empty list */
      return 0;
    }

    while(temp != NULL) {
      count++;
    	temp = temp->next;
    }

    return count;
}
