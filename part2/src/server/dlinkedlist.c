#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "user_storage.h"
#include "dlinkedlist.h"

/* Head of the Doubly Linked List*/
// Node* head;

pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER;

/* Deletes all the list and frees the memory */
int destroyList(Node* head) {
    pthread_mutex_lock(&list_lock);
    if(head == NULL) {
      pthread_mutex_unlock(&list_lock);
      return 0;
    }

    Node* node = head;
    Node* temp;
    while(node != NULL) {
        temp = node;
        node = node->next;
        free(temp);
    }

    head = NULL;
    pthread_mutex_unlock(&list_lock);
    pthread_mutex_destroy(&list_lock);

    return 0;
}

/* Method to create a new node */
Node* getNewNode(void* data) {
    Node* newNode = (Node*)malloc(sizeof(Node));

    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}

/* Method to print in stdout the current nodes and their values (debugging) */
// void printList(Node* head) {
//   pthread_mutex_lock(&list_lock);
// 	Node* temp = head;
//   int counter = 0;
// 	while(temp != NULL) {
// 		printf("NODE %d: %s, status: %d, %s:%d\n", counter, temp->data->username, temp->data->status, inet_ntoa(*(temp->data->ip_address)), temp->data->port);
// 		temp = temp->next;
//     counter++;
// 	}
//   pthread_mutex_unlock(&list_lock);
// }

/* Method to insert a new node in the list. This node will be added at the end */
int insert_user(Node* head, Node* newNode) {
    pthread_mutex_lock(&list_lock);
    Node* temp = head;

    if(head == NULL) {
        head = newNode;
        pthread_mutex_unlock(&list_lock);
        return 0;
    }

    while(temp != NULL) {
        if(strcmp(((struct user*)temp->data)->username, ((struct user*)newNode->data)->username) == 0) {
            /* Key already exists */
            pthread_mutex_unlock(&list_lock);
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
    pthread_mutex_unlock(&list_lock);
    return 0;
}

/* Method to insert a new node in the list. This node will be added at the end */
int insert_msg(Node* head, Node* newNode) {
    pthread_mutex_lock(&list_lock);
    Node* temp = head;

    if(head == NULL) {
        head = newNode;
        pthread_mutex_unlock(&list_lock);
        return 0;
    }

    while(temp != NULL) {
        if(temp->next != NULL) {
            temp = temp->next;
        } else {
            break;
        }
    }

    temp->next = newNode;
    newNode->prev = temp;
    pthread_mutex_unlock(&list_lock);
    return 0;
}


/* Deletes the node with the provided key */
int delete_user(Node* head, char* username) {
    pthread_mutex_lock(&list_lock);
    Node* temp = head;

    if(head == NULL) {
        pthread_mutex_unlock(&list_lock);
        return -1;
    }

    while(temp != NULL) {
        if(strcmp(((struct user*)temp->data)->username, username) == 0) {
            if(temp == head) {
                head = temp->next;
                free(temp);
                pthread_mutex_unlock(&list_lock);
                return 0;
            }

            Node* prev = temp->prev;
            Node* next = temp->next;

            prev->next = temp->next;
            next->prev = temp->prev;

            free(temp);
            pthread_mutex_unlock(&list_lock);
            return 0;
        }
        temp = temp->next;
    }
    pthread_mutex_unlock(&list_lock);
    return -1;
}

int delete_msg(Node* head, unsigned int id) {
    pthread_mutex_lock(&list_lock);
    Node* temp = head;

    if(head == NULL) {
        pthread_mutex_unlock(&list_lock);
        return -1;
    }

    while(temp != NULL) {
        if(((struct message*)temp->data)->id == id) {
            if(temp == head) {
                head = temp->next;
                free(temp);
                pthread_mutex_unlock(&list_lock);
                return 0;
            }

            Node* prev = temp->prev;
            Node* next = temp->next;

            prev->next = temp->next;
            next->prev = temp->prev;

            free(temp);
            pthread_mutex_unlock(&list_lock);
            return 0;
        }
        temp = temp->next;
    }
    pthread_mutex_unlock(&list_lock);
    return -1;
}

/* Returns the node with the provided key associated */
Node* search_user(Node* head, char* username) {
    pthread_mutex_lock(&list_lock);
    Node* temp = head;

    if(head == NULL) {
        pthread_mutex_unlock(&list_lock);
        return NULL;
    }

    while(temp != NULL) {
      if(strcmp(((struct user*)temp->data)->username, username) == 0) {
            pthread_mutex_unlock(&list_lock);
            return temp;
        }
	      temp = temp->next;
    }
    pthread_mutex_unlock(&list_lock);
    return NULL;
}

/* Returns the node with the provided key associated */
Node* search_msg(Node* head, unsigned int id) {
    pthread_mutex_lock(&list_lock);
    Node* temp = head;

    if(head == NULL) {
        pthread_mutex_unlock(&list_lock);
        return NULL;
    }

    while(temp != NULL) {
      if(((struct message*)temp->data)->id == id) {
            pthread_mutex_unlock(&list_lock);
            return temp;
        }
	      temp = temp->next;
    }
    pthread_mutex_unlock(&list_lock);
    return NULL;
}


int modify_user(Node* head, Node* newNode) {
    pthread_mutex_lock(&list_lock);
    Node* temp = head;

    if(head == NULL) {
        /* Empty list */
        pthread_mutex_unlock(&list_lock);
        return -1;
    }

    while(temp != NULL) {
      if(strcmp(((struct user*)temp->data)->username, ((struct user*)newNode->data)->username) == 0) {
            /* Key found */
            temp->data = newNode->data;
            pthread_mutex_unlock(&list_lock);
            return 0;
        }

        if(temp->next != NULL) {
            temp = temp->next;
        } else {
            break;
        }
    }
    /* Key not found */
    pthread_mutex_unlock(&list_lock);
    return -1;
}

int modify_msg(Node* head, Node* newNode) {
    pthread_mutex_lock(&list_lock);
    Node* temp = head;

    if(head == NULL) {
        /* Empty list */
        pthread_mutex_unlock(&list_lock);
        return -1;
    }

    while(temp != NULL) {
      if(((struct message*)temp->data)->id == ((struct message*)newNode->data)->id) {
            /* Key found */
            temp->data = newNode->data;
            pthread_mutex_unlock(&list_lock);
            return 0;
        }

        if(temp->next != NULL) {
            temp = temp->next;
        } else {
            break;
        }
    }
    /* Key not found */
    pthread_mutex_unlock(&list_lock);
    return -1;
}

/* Returns the number of nodes currently stored in the list */
unsigned int getCardinality(Node* head) {
    pthread_mutex_lock(&list_lock);

    Node* temp = head;
    unsigned int count = 0;

    if(head == NULL) {
      /* Empty list */
      pthread_mutex_unlock(&list_lock);
      return 0;
    }

    while(temp != NULL) {
      count++;
    	temp = temp->next;
    }

    pthread_mutex_unlock(&list_lock);
    return count;
}

int get_total_messages_by_user(Node* head, char* username) {
  pthread_mutex_lock(&list_lock);
  Node* temp = head;
  int count = 0;

  if (head == NULL) {
    pthread_mutex_unlock(&list_lock);
    return 0;
  }

  while(temp != NULL) {
    if (strcmp(((struct message*)temp->data)->from_user, username) == 0) {
      count++;
    }
    temp = temp->next;
  }

  pthread_mutex_unlock(&list_lock);
  return count;
}
