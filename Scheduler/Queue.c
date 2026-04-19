#include <stdio.h>
#include <stdlib.h>

#include "Includes.h"

Queue *create_queue() {
    Queue *q = malloc(sizeof(Queue));
    if (q == NULL) return NULL;
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
    return q;
}

// Add to back
void enqueue(Queue *q, int process_id) {
    QueueNode *node = malloc(sizeof(QueueNode));
    if (node == NULL) return;
    node->process_id = process_id;
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
int dequeue(Queue *q) {
    if (q->front == NULL) return -1;

    QueueNode *temp = q->front;
    int process = temp->process_id;
    q->front = q->front->next;

    if (q->front == NULL)        // queue is now empty
        q->rear = NULL;

    free(temp);
    q->size--;
    return process;
}

// Peek without removing
int peek(Queue *q) {
    if (q->front == NULL) return -1;
    return q->front->process_id;
}

int is_empty(Queue *q) {
    return q->size == 0;
}
