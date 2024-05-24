#ifndef QUEUE_h
#define QUEUE_h

typedef struct {
    int *data;
    int size;
    int capacity;
    int front;
    int rear;
} Queue;


#endif //QUEUE_h