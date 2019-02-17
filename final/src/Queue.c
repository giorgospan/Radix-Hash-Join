#include <stdio.h>
#include <stdlib.h>

#include "Queue.h"
#include "Utils.h"

void createQueue(struct Queue **q, int size)
{
	/* Dynamic */
	*q          = malloc(sizeof(struct Queue));
	MALLOC_CHECK(*q);
	(*q)->array = malloc(sizeof(int) * size);
	MALLOC_CHECK((*q)->array);

	/* Data for ds */
	(*q)->size = size;
	(*q)->front = -1;
	(*q)->rear = -1;

}

void destroyQueue(struct Queue *q)
{
	free(q->array);
	free(q);
}

int enQueue(struct Queue *q, void* item)
{
	// fprintf(stderr,"item %p\n", item);
	if ( ((q->rear == q->size - 1) && (q->front == 0)) || (q->rear == (q->front - 1) % (q->size - 1)) )
	{
		// display(q);
		fprintf(stderr, "%s\n", "Circular Queue is full");
		return 0;
	}
	/* This is the first item inserted */
	else if (q->front == -1)
	{
		q->rear     = 0;
		q->front    = 0;
		q->array[0] = item;
	}
	/* When the rear has reached the end but there is still space in the array
		we just move rear to the begining of the queue */
	else if ( (q->rear == q->size - 1) && (q->front != 0))
	{
		q->rear           = 0;
		q->array[q->rear] = item;
	}
	/* just insert in the next position */
	else
		q->array[++(q->rear)] = item;
}

int isEmpty(struct Queue *q){
	return (q->front == -1);
}

void* deQueue(struct Queue *q)
{
	if (q->front == -1)
	{
		fprintf(stderr, "%s\n", "Circular Queue is empty");
		return 0;
	}

	void* value = q->array[q->front];

	/* We had an one item queue if this is true */
	if (q->front == q->rear)
		q->front = q->rear = -1;
	/* we have reached the end , so we start extracting from the start again */
	else if (q->front == q->size - 1)
		q->front = 0;
	/* we just go to the next element */
	else
		++(q->front);

	return value;
}


void display(struct Queue *q)
{
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	fprintf(stderr,"front is :%d \n", q->front);
	fprintf(stderr,"rear is :%d \n", q->rear);
	for (int i = 0; i < q->size; i++)
		fprintf(stderr,"%p| ", q->array[i]);
	fprintf(stderr,"\n");
	fprintf(stderr,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}
