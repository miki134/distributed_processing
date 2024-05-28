#ifndef QUEUE_h
#define QUEUE_h

#include <pthread.h>

typedef struct {
    int *data;
    int size;
    int capacity;
    int front;
    int rear;
    pthread_mutex_t queueMut;
} Queue;

void initQueue(Queue *q, int initialCapacity);
void enqueue(Queue *q, int element);
int dequeue(Queue *q);
void printQueue(const Queue *q);
void freeQueue(Queue *q);
int example();
int getSize(Queue *q);

void lockMutex(Queue *q);
void unlockMutex(Queue *q);


#endif //QUEUE_h