#include <stdio.h>
#include <stdlib.h>

#include "includes.h"

Queue *create_queue() {
    Queue *q = malloc(sizeof(Queue));
    if (q == NULL) return NULL;
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
    return q;
}

// Add to back
void enqueue(Queue *q, PCB *process) {
    QueueNode *node = malloc(sizeof(QueueNode));
    if (node == NULL) return;
    node->process = process;
    node->next = NULL;

    if (q->rear == NULL) {       // queue is empty
        q->front = node;
        q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
    q->size++;
}

// Remove from front
PCB *dequeue(Queue *q) {
    if (q->front == NULL) return NULL;

    QueueNode *temp = q->front;
    PCB *process = temp->process;
    q->front = q->front->next;

    if (q->front == NULL)        // queue is now empty
        q->rear = NULL;

    free(temp);
    q->size--;
    return process;
}

// Peek without removing
PCB *peek(Queue *q) {
    if (q->front == NULL) return NULL;
    return q->front->process;
}

int is_empty(Queue *q) {
    return q->size == 0;
}
