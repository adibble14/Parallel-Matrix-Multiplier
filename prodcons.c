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
int fill = 0;
int use = 0;
int count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t putCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t getCond = PTHREAD_COND_INITIALIZER;



// Bounded buffer put() get()
int put(Matrix * value)
{
  bigmatrix[fill] = value;
  fill = (fill + 1) % BOUNDED_BUFFER_SIZE;
  count++;
  printf("putting>>>> Count %d\n", count);
  pthread_cond_signal(&getCond);
  return count;
}

Matrix * get()
{
  Matrix * tmp = bigmatrix[use];
  use = (use + 1) % BOUNDED_BUFFER_SIZE;
  count--;
  printf("getting>>>> Count %d\n", count);
  pthread_cond_signal(&putCond);
  return tmp;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
  counter_t * counter =   (counter_t *) arg;

  ProdConsStats * stats = malloc(sizeof(ProdConsStats));
  stats->matrixtotal = 0;
  stats->multtotal = 0;
  stats->sumtotal = 0;

  //get_set increments the counter by 1 and returns the value
  while(get_set(counter) < NUMBER_OF_MATRICES )
  {
    Matrix * matrix = GenMatrixRandom();
    pthread_mutex_lock(&mutex);
    while(count == BOUNDED_BUFFER_SIZE)
    {
      printf("waiting\n");
        pthread_cond_wait(&putCond, &mutex);
    }
    put(matrix);
    pthread_mutex_unlock(&mutex);
    stats->sumtotal += SumMatrix(matrix);
    stats->matrixtotal++;
  }
  printf(" Producer total matrix: %d\n", stats->matrixtotal);
  return (void *) stats;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
  counter_t * counter =   (counter_t *) arg;
  ProdConsStats * stats = malloc(sizeof(ProdConsStats));
  stats->matrixtotal = 0;
  stats->multtotal = 0;
  stats->sumtotal = 0;

  while(get_set(counter) < NUMBER_OF_MATRICES){
    pthread_mutex_lock(&mutex);
    while(count == 0) {
        pthread_cond_wait(&getCond, &mutex);
    }

    Matrix* m1 = get();
    pthread_mutex_unlock(&mutex);
    stats->matrixtotal++;
    stats->sumtotal += SumMatrix(m1);
    
    Matrix * m2 = NULL;
    Matrix* m3 = NULL;

    while (m3 == NULL && get_set(counter) < NUMBER_OF_MATRICES) {
      pthread_mutex_lock(&mutex);
      while(count == 0) {
        pthread_cond_wait(&getCond, &mutex);
      }
      m2 = get();
      pthread_mutex_unlock(&mutex);
      stats->matrixtotal++;
      stats->sumtotal += SumMatrix(m2);

      m3 = MatrixMultiply(m1, m2);
      stats->multtotal += 2;
      if (m3 == NULL) {
        FreeMatrix(m2);
        m2 = NULL;
      }
    }
    if(m3 != NULL)
    {
      FreeMatrix(m3);
    }
    if(m2 != NULL)
    {
      FreeMatrix(m2);
    }
    if(m1 != NULL)
    {
      FreeMatrix(m1);
    }
  }
  printf("Consumer total matrix: %d\n", stats->matrixtotal);
  return (void *) stats;
}
