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
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t putCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t getCond = PTHREAD_COND_INITIALIZER;
int count = 0;


// Bounded buffer put() get()
int put(Matrix * value)
{
  bigmatrix[fill] = value;
  fill = (fill + 1) % BOUNDED_BUFFER_SIZE;
  count++;
  return count;
}

Matrix * get()
{
  Matrix * tmp = bigmatrix[use];
  use = (use + 1) % BOUNDED_BUFFER_SIZE;
  count--;
  return tmp;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
  printf("Staring production>>>\n");
  ProdConsStats * stats = malloc(sizeof(ProdConsStats));
  stats->matrixtotal = 0;
  stats->multtotal = 0;
  stats->sumtotal = 0;
  int i;
  for(i = 0; i < NUMBER_OF_MATRICES; i++)
  {
    Matrix * matrix = GenMatrixRandom();
    pthread_mutex_lock(&mutex);
    while(count == BOUNDED_BUFFER_SIZE)
    {
        pthread_cond_wait(&putCond, &mutex);
    }
    put(matrix);
    pthread_cond_signal(&getCond);
    pthread_mutex_unlock(&mutex);
    FreeMatrix(matrix);
    stats->matrixtotal++;
  }
  printf(" Producer total matrix: %d\n", stats->matrixtotal);
  return (void *) stats;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
  printf("Starting Consumption>>>>\n");
  //ProdConsStats stats = {0, 0, 0};
  ProdConsStats * stats = malloc(sizeof(ProdConsStats));
  stats->matrixtotal = 0;
  stats->multtotal = 0;
  stats->sumtotal = 0;
  for (int i = 0; i < NUMBER_OF_MATRICES; i++) {
    pthread_mutex_lock(&mutex);
    while(count == 0) {
        pthread_cond_wait(&getCond, &mutex);
    }
    Matrix* m1 = get();
    printf("m1\n");
    DisplayMatrix(m1, stdout);
    pthread_cond_signal(&putCond);
    pthread_mutex_unlock(&mutex);

    Matrix * m2 = NULL;
    Matrix* m3 = NULL;
    while (m3 == NULL) {
      pthread_mutex_lock(&mutex);
      while(count == 0) {
        pthread_cond_wait(&getCond, &mutex);
      }
      m2 = get();
      pthread_cond_signal(&putCond);
      pthread_mutex_unlock(&mutex);
      m3 = MatrixMultiply(m1, m2);
      if (m3 == NULL) {
        FreeMatrix(m2);
      }
    }
    FreeMatrix(m2);
    FreeMatrix(m1);
    stats->matrixtotal++;
  }
  printf("Consumer total matrix: %d\n", stats->matrixtotal);
  return (void *) stats;
}
