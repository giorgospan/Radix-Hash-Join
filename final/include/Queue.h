#ifndef QUEUE_H
#define QUEUE_H

struct Queue
{
	/* Start and end of queue */
	int front;
	int rear;

	/* Fixed size of array */
	int size;
	void **array;
};

/* Creates the queue */
void createQueue(struct Queue **q, int size);

/* Free the allocated memory of the data structure */
void destroyQueue(struct Queue *q);

/* enQueue an item returns 1 on success */
int enQueue(struct Queue *q, void* item);

/* always extract from front  returns item on success */
void* deQueue(struct Queue *q);

int isEmpty(struct Queue *q);

/* display queue and front and rear */
void display(struct Queue *q);


#endif
