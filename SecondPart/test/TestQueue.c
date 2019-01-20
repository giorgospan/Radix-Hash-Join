/**
 * Unit testing on queue functions [Queue.c]
 */
#include <stdio.h>
#include "CUnit/Basic.h"
#include "Queue.h"
#include "Utils.h"

/* Also tests isEmpty(..) function */
void testCreateQueue(void)
{
    struct Queue *q;
    createQueue(&q,20);
    CU_ASSERT_EQUAL(q->front,q->rear);
    CU_ASSERT_EQUAL(isEmpty(q),1);
    destroyQueue(q);
}


void testEnqueue(void)
{
  struct Queue *q;
  createQueue(&q,10);
  enQueue(q,(void*)1);
  enQueue(q,(void*)4);
  enQueue(q,(void*)10);
  enQueue(q,(void*)2);
  CU_ASSERT_EQUAL(q->rear,3);
  CU_ASSERT_EQUAL(q->front,0);
  CU_ASSERT_EQUAL(isEmpty(q),0);
  destroyQueue(q);
}


void testDeQueue(void)
{
  struct Queue *q;
  createQueue(&q,10);
  enQueue(q,(void*)1);
  enQueue(q,(void*)4);

  deQueue(q);
  CU_ASSERT_EQUAL(q->rear,1);
  CU_ASSERT_EQUAL(q->front,1);

  deQueue(q);
  CU_ASSERT_EQUAL(q->rear,-1);
  CU_ASSERT_EQUAL(q->front,-1);


  CU_ASSERT_EQUAL(isEmpty(q),1);
  destroyQueue(q);
}
