#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#include "user_storage.h"

Queue *ConstructQueue(int limit);
void DestructQueue(Queue *queue);
int Enqueue(Queue *pQueue, NODE *item);
NODE *Dequeue(Queue *pQueue);
int isEmpty(Queue* pQueue);

#endif /* QUEUE */
