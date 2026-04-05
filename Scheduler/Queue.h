#ifndef QUEUE_H
#define QUEUE_H

#include "Process Controller.h"

typedef struct QueueNode {
    PCB *process;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
    int size;
} Queue;

Queue *create_queue();
void enqueue(Queue *q, PCB *process);
PCB *dequeue(Queue *q);
PCB *peek(Queue *q);
int is_empty(Queue *q);

#endif