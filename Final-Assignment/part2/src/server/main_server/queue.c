#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "user_storage.h"
#include "queue.h"

pthread_mutex_t lock;

/* a link in the queue, holds the info and point to the next Node*/
// typedef struct {
//     //int info;
//     struct message* mes;
// } DATA;
//
// typedef struct Node_t {
//    DATA data;
//    struct Node_t *prev;
// } NODE;
//
// /* the HEAD of the Queue, hold the amount of node's that are in the queue*/
// typedef struct Queue {
//     NODE *head;
//     NODE *tail;
//     int size;
//     int limit;
// } Queue;

Queue *ConstructQueue(int limit);
void DestructQueue(Queue *queue);
int Enqueue(Queue *pQueue, NODE *item);
NODE *Dequeue(Queue *pQueue);
int isEmpty(Queue* pQueue);

Queue *ConstructQueue(int limit) {
    if(pthread_mutex_init(&lock, NULL) != 0){
      return NULL;
    }
    pthread_mutex_lock(&lock);
    Queue *queue = (Queue*) malloc(sizeof (Queue));
    if (queue == NULL) {
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    if (limit <= 0) {
        limit = 10;
    }
    queue->limit = limit;
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;

    pthread_mutex_unlock(&lock);
    return queue;
}

void DestructQueue(Queue *queue) {
    pthread_mutex_lock(&lock);
    NODE *pN;
    while (!isEmpty(queue)) {
        pN = Dequeue(queue);
        free(pN);
    }
    free(queue);

    pthread_mutex_unlock(&lock);
    pthread_mutex_destroy(&lock);
}

int Enqueue(Queue *pQueue, NODE *item) {
    pthread_mutex_lock(&lock);

    /* Bad parameter */
    if ((pQueue == NULL) || (item == NULL)) {
        pthread_mutex_unlock(&lock);
        return FALSE;
    }
    // if(pQueue->limit != 0)
    if (pQueue->size >= pQueue->limit) {
        pthread_mutex_unlock(&lock);
        return FALSE;
    }
    /*the queue is empty*/
    item->prev = NULL;
    if (pQueue->size == 0) {
        pQueue->head = item;
        pQueue->tail = item;

    } else {
        /*adding item to the end of the queue*/
        pQueue->tail->prev = item;
        pQueue->tail = item;
    }
    pQueue->size++;

    pthread_mutex_unlock(&lock);
    return TRUE;
}

NODE * Dequeue(Queue *pQueue) {
    pthread_mutex_lock(&lock);
    /*the queue is empty or bad param*/
    NODE *item;
    if (isEmpty(pQueue)) {
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    item = pQueue->head;
    pQueue->head = (pQueue->head)->prev;
    pQueue->size--;

    pthread_mutex_unlock(&lock);
    return item;
}

int isEmpty(Queue* pQueue) {
    // pthread_mutex_lock(&lock);
    if (pQueue == NULL) {
        // pthread_mutex_unlock(&lock);
        return FALSE;
    }
    if (pQueue->size == 0) {
        // pthread_mutex_unlock(&lock);
        return TRUE;
    } else {
        // pthread_mutex_unlock(&lock);
        return FALSE;
    }
}
