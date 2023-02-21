/*
 *  prodcons module
 *  Producer Consumer module
 *
 *  Implements routines for the producer consumer module based on
 *  chapter 30, section 2 of Operating Systems: Three Easy Pieces
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 */

// Include only libraries for this module
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "counter.h"
#include "matrix.h"
#include "pcmatrix.h"
#include "prodcons.h"


// Define Locks, Condition variables, and so on here
//Matrix ** bigmatrix = (Matrix **) malloc(sizeof(Matrix *) * BOUNDED_BUFFER_SIZE);
int fill = 0;
int use = 0;
pthread_mutex_t putMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t putCond = PTHREAD_COND_INITIALIZER;
int count = 0;


// Bounded buffer put() get()
int put(Matrix * value)
{
  bigmatrix = bigmatrix + fill;
  bigmatrix = value;
  fill = (fill + 1) % BOUNDED_BUFFER_SIZE;
  count++;
  return count;
}

Matrix * get()
{
  bigmatrix = bigmatrix + use;
  Matrix * tmp = &bigmatrix;
  use = (use + 1) % BOUNDED_BUFFER_SIZE;
  count--;
  return tmp;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
  //stats
  ProdConsStats stats = {0, 0, 0};
  int i;
  for(i = 0; i < NUMBER_OF_MATRICES; i++)
  {
    Matrix * matrix = GenMatrixRandom();
    pthread_mutex_lock(&putMutex);
<<<<<<< HEAD
    while(count == 0) // buffer size
=======
    while(produce == BOUNDED_BUFFER_SIZE) // buffer size
>>>>>>> 9e8ffe6 (Fixed while loop.)
    {
        pthread_cond_wait(&putCond, &putMutex);
    }
    put(matrix);

    pthread_mutex_unlock(&putMutex);

  }
  
  return NULL;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
  return NULL;
}
