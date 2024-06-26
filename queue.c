#include "queue.h"
#include "util.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void initQueue(Queue *q, int initialCapacity)
{
    q->data = (int *)malloc(initialCapacity * sizeof(int));
    if (q->data == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    q->size = 0;
    q->capacity = initialCapacity;
    q->front = 0;
    q->rear = -1;

    if (pthread_mutex_init(&q->queueMut, NULL) != 0)
    {
        fprintf(stderr, "Mutex initialization failed\n");
        exit(1);
    }
}

void enqueue(Queue *q, int element)
{
    // printQueue(q);
    pthread_mutex_lock(&q->queueMut);
    if (q->size == q->capacity)
    {
        q->capacity *= 2;
        int *temp = (int *)realloc(q->data, q->capacity * sizeof(int));
        if (temp == NULL)
        {
            free(q->data);
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        q->data = temp;

        if (q->rear < q->front)
        {
            if (q->rear + 1 < q->front)
            {
                int end_size = q->rear + 1;
                memmove(q->data + q->capacity - end_size, q->data, end_size * sizeof(int));
                q->rear = q->capacity - 1;
            }
        }
    }
    q->rear = (q->rear + 1) % q->capacity;
    q->data[q->rear] = element;
    q->size++;
    pthread_mutex_unlock(&q->queueMut);
}

int dequeue(Queue *q)
{
    // printQueue(q);
    pthread_mutex_lock(&q->queueMut);
    if (q->size == 0)
    {
        fprintf(stderr, "Queue underflow\n");
        exit(1);
    }
    int element = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    pthread_mutex_unlock(&q->queueMut);
    return element;
}

void printQueue(const Queue *q)
{
    // pthread_mutex_lock(&q->queueMut);
    pthread_mutex_lock(&q->queueMut);
    if (q->size > 0)
    {
        printf("size: %d -- ", q->size);
        for (int i = 0; i < q->size; i++)
        {
            printf("%d ", q->data[(q->front + i) % q->capacity]);
        }
        printf("\n");
    }
    pthread_mutex_unlock(&q->queueMut);
}

char* toString(const Queue *q) {
    pthread_mutex_lock(&q->queueMut);

    int buffer_size = 50 + (q->size * 12);
    char *result = (char *)malloc(buffer_size);
    if (!result) {
        pthread_mutex_unlock(&q->queueMut);
        return NULL;
    }

    // int offset = snprintf(result, buffer_size, "size: %d -- ", q->size);
    int offset = snprintf(result, buffer_size, "");
    for (int i = 0; i < q->size; i++) {
        offset += snprintf(result + offset, buffer_size - offset, "%d ", q->data[(q->front + i) % q->capacity]);
    }
    snprintf(result + offset, buffer_size - offset, "");

    pthread_mutex_unlock(&q->queueMut);

    return result;
}

void freeQueue(Queue *q)
{
    // pthread_mutex_lock(&q->queueMut);
    if (q->size > 0)
    {
        free(q->data);
        q->data = NULL;
        q->size = 0;
        q->capacity = 0;
        q->front = 0;
        q->rear = -1;
    }
    // pthread_mutex_unlock(&q->queueMut);
}

int getSize(Queue *q)
{
    pthread_mutex_lock(&q->queueMut);
    int s = q->size;
    pthread_mutex_unlock(&q->queueMut);
    return s;
}

void lockMutex(Queue *q)
{
    pthread_mutex_lock(&q->queueMut);
}

void unlockMutex(Queue *q)
{
    pthread_mutex_unlock(&q->queueMut);
}

// void removeElement(Queue *q, int x)
// {
//     pthread_mutex_lock(&q->queueMut);
//     int found = 0;
//     for (int i = 0; i < q->size; i++)
//     {
//         int idx = (q->front + i) % q->capacity;
//         if (q->data[idx] == x)
//         {
//             found = 1;
//         }
//         if (found && i < q->size - 1)
//         {
//             int nextIdx = (q->front + i + 1) % q->capacity;
//             q->data[idx] = q->data[nextIdx];
//         }
//     }
//     if (found)
//     {
//         q->rear = (q->rear - 1 + q->capacity) % q->capacity;
//         q->size--;
//     }
//     pthread_mutex_unlock(&q->queueMut);
// }
void removeElement(Queue *q, int x)
{
    pthread_mutex_lock(&q->queueMut);
    int newSize = 0;
    for (int i = 0; i < q->size; i++)
    {
        int idx = (q->front + i) % q->capacity;
        if (q->data[idx] != x)
        {
            q->data[(q->front + newSize) % q->capacity] = q->data[idx];
            newSize++;
        }
    }
    q->size = newSize;
    q->rear = (q->front + q->size - 1) % q->capacity;
    pthread_mutex_unlock(&q->queueMut);
}

int example()
{
    Queue q;
    initQueue(&q, 10);
    printQueue(&q);

    for (int i = 0; i < 20; i++)
    {
        enqueue(&q, i);
    }
    for (int i = 0; i < 20; i++)
    {
        enqueue(&q, i);
    }
    printQueue(&q);

    removeElement(&q, 123);
    removeElement(&q, 4);
    printQueue(&q);

    for (int i = 0; i < 10; i++)
    {
        printf("Dequeued: %d\n", dequeue(&q));
    }

    printQueue(&q);

    freeQueue(&q);

    return 0;
}
