#include "queue.h"
#include "usefulHeaders.h"


void createQueue(struct queue **q, int size)
{
	/* Dynamic */
	*q = malloc(sizeof(struct queue));
	(*q)->array = malloc(sizeof(int) * size);

	/* Data for ds */
	(*q)->size = size;
	(*q)->front = -1;
	(*q)->rear = -1;

	/* Valgrind warns of unintialized values should be removed */
	for (int i = 0; i < size; i++)
		(*q)->array[i] = 0;
}

void destroyQueue(struct queue *q)
{
	free(q->array);
	free(q);
}

int enQueue(struct queue *q, int item)
{
	printf("item %d\n", item);
	if ( ((q->rear == q->size - 1) && (q->front == 0)) || (q->rear == (q->front - 1) % (q->size - 1)) )
	{
		// display(q);	
		fprintf(stderr, "%s\n", "Circular Queue is full");
		return 0;
	}
	/* This is the first item inserted */
	else if (q->front == -1)
	{
		q->rear = 0;
		q->front = 0;
		q->array[0] = item;
	}
	/* When the rear has reached the end but there is still space in the array
		we just move rear to the begining of the queue */
	else if ( (q->rear == q->size - 1) && (q->front != 0))
	{
		q->rear = 0;
		q->array[q->rear] = item;
	}
	/* just insert in the next position */
	else
		q->array[++(q->rear)] = item;
}


int deQueue(struct queue *q)
{
	if (q->front == -1)
	{
		fprintf(stderr, "%s\n", "Circular Queue is empty");
		return 0;
	}

	int value = q->array[q->front];

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


void display(struct queue *q)
{
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("front is :%d \n", q->front);
	printf("rear is :%d \n", q->rear);
	for (int i = 0; i < q->size; i++)
		printf("%d| ", q->array[i]);
	printf("\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}