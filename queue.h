#ifndef QUEUE_H
#define QUEUE_H

#include "usefulHeaders.h"

struct queue
{
	/* Start and end of queue */
	int front;
	int rear;

	/* Fixed size of array */
	int size;
	int *array;
};

/* Creates the queue */
void createQueue(struct queue **q, int size);

/* Free the allocated memory of the data structure */
void destroyQueue(struct queue *q);

/* enQueue an item returns 1 on success */
int enQueue(struct queue *q, int item);

/* always extract from front  returns item on success */
int deQueue(struct queue *q);

/* display queue and front and rear */
void display(struct queue *q);


#endif