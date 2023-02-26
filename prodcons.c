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
pthread_mutex_t getMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t putCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t getCond = PTHREAD_COND_INITIALIZER;
int count = 0;
int test = 0;


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
  if(count <= 0){
	return tmp;
  }else{
    count--;
  }
  test++;
  printf("getting>>>> Count %d\n", count);
  pthread_cond_signal(&putCond);
  return tmp;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
  printf("Staring production>>>\n");
  ProdConsStats * stats = malloc(sizeof(ProdConsStats));
  //assert(stats != NULL);
  stats->matrixtotal = 0;
  stats->multtotal = 0;
  stats->sumtotal = 0;
  int i;
  for(i = 0; i < NUMBER_OF_MATRICES; i++)
  {
    Matrix * matrix = GenMatrixRandom();
    pthread_mutex_lock(&putMutex);
    while(count == BOUNDED_BUFFER_SIZE)
    {
        pthread_cond_wait(&putCond, &putMutex);
    }
    put(matrix);
    pthread_mutex_unlock(&putMutex);
    stats->sumtotal += SumMatrix(matrix);
    stats->matrixtotal++;
  }
  printf(" Producer total matrix: %d\n", stats->matrixtotal);
  return (void *) stats;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
  printf("Starting Consumption>>>>\n");
  ProdConsStats * stats = malloc(sizeof(ProdConsStats));
  //assert(stats != NULL);
  stats->matrixtotal = 0;
  stats->multtotal = 0;
  stats->sumtotal = 0;
  while(test < NUMBER_OF_MATRICES){
    pthread_mutex_lock(&getMutex);
    while(count == 0 && test < NUMBER_OF_MATRICES) {
        pthread_cond_wait(&getCond, &getMutex);
    }

    //checking to see if all matrixes have been consumed
    if(count == 0 && test >= NUMBER_OF_MATRICES-1){
      	return (void *) stats;
      }
    Matrix* m1 = get();
    stats->sumtotal += SumMatrix(m1);
    printf("m1 %p\n", &m1);
    DisplayMatrix(m1, stdout);
    pthread_mutex_unlock(&getMutex);
    
    //checking to see if all matrixes have been consumed
    if(count == 0 && test >= NUMBER_OF_MATRICES-1){
      	return (void *) stats;
      }

    Matrix * m2 = NULL;
    Matrix* m3 = NULL;
    while (m3 == NULL) {
      pthread_mutex_lock(&getMutex);
      while(count == 0 && test < NUMBER_OF_MATRICES) {
        pthread_cond_wait(&getCond, &getMutex);
      }
      
      //checking to see if all matrixes have been consumed
      if(count == 0 && test >= NUMBER_OF_MATRICES-1){
      	return (void *) stats;
      }
      m2 = get();
      stats->sumtotal += SumMatrix(m2);
      printf("m2 %p\n", &m2);
    DisplayMatrix(m2, stdout);
      pthread_mutex_unlock(&getMutex);
      m3 = MatrixMultiply(m1, m2);
      stats->multtotal += 2;
      printf("m3 %p\n", &m3);
       DisplayMatrix(m3, stdout);
      if (m3 == NULL) {
        FreeMatrix(m2);
      }
    }
    FreeMatrix(m3);
    FreeMatrix(m2);
    FreeMatrix(m1);
    stats->matrixtotal++;
  }
  printf("Consumer total matrix: %d\n", stats->matrixtotal);
  return (void *) stats;
}
