#ifndef QUEUE_H
#define QUEUE_H

typedef struct QueueNode {
    int process_id; // I should be using a void*, but I don't want the hassle of casting
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
    int size;
} Queue;

Queue *create_queue();
void enqueue(Queue *q, int process_id);
int dequeue(Queue *q);
int peek(Queue *q);
int is_empty(Queue *q);

#endif