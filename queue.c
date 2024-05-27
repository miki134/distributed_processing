#include "queue.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

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
}

void enqueue(Queue *q, int element)
{
    pthread_mutex_lock(&queueMut);
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
    pthread_mutex_unlock(&queueMut);
}

int dequeue(Queue *q)
{
    pthread_mutex_lock(&queueMut);
    if (q->size == 0)
    {
        fprintf(stderr, "Queue underflow\n");
        exit(1);
    }
    int element = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    pthread_mutex_unlock(&queueMut);
    return element;
}

void printQueue(const Queue *q)
{
    printf("size: %d\n", q->size);
    for (int i = 0; i < q->size; i++)
    {
        printf("%d ", q->data[(q->front + i) % q->capacity]);
    }
    printf("\n");
}

void freeQueue(Queue *q)
{
    free(q->data);
    q->data = NULL;
    q->size = 0;
    q->capacity = 0;
    q->front = 0;
    q->rear = -1;
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

    printQueue(&q);

    for (int i = 0; i < 10; i++)
    {
        printf("Dequeued: %d\n", dequeue(&q));
    }

    printQueue(&q);

    freeQueue(&q);

    return 0;
}
