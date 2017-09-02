/*
Author: Shimi Smith
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAXSIZE 50

typedef uint32_t Data;

typedef struct Node{
	Data data;
} Node;

typedef struct Queue{
	int currSize;
	int front;
	int rear;
	Node queueArray[MAXSIZE];
} Queue;

Queue initQueue();
int isFull(Queue *Q);
int isEmpty(Queue *Q);
void enqueue(Queue *Q, Data data);
Data dequeue(Queue *Q);