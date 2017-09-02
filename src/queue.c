/*
Author: Shimi Smith

This queue implementation doesn't use malloc or free to adhere to the cubesat coding rules

EXAMPLE OF USE
--------------
Queue q = initQueue();
Queue *Q = &q;

enqueue(Q, 1);

printf("%d\n", dequeue(Q)); --> prints 1

*/

#include "queue.h"

Queue initQueue(){
	Queue Q;
	Q.currSize = 0;
	Q.front = 0;
	Q.rear = 0;

	return Q;
}

int isFull(Queue *Q){
	return Q->currSize == MAXSIZE;
}

int isEmpty(Queue *Q){
	return Q->currSize == 0;
}

void enqueue(Queue *Q, Data data){
	if(!isFull(Q)){
		Q->queueArray[Q->rear].data = data;
		Q->rear = (Q->rear + 1) % MAXSIZE;
		Q->currSize++;
	}
}

Data dequeue(Queue *Q){
	if(!isEmpty(Q)){
		Data data = Q->queueArray[Q->front].data;
		Q->front = (Q->front + 1) % MAXSIZE;
		Q->currSize--;
		return data;
	}
	return 0;
}