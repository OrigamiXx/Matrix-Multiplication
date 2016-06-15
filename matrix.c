#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include "matrix.h"
#include "permutation.h"
#include "CheckUSP.h"
#include "groups.h"
#include "constants.h"

// Constructor. Create a zero matrix.
mat * create_mat_zero(int rows, int cols){

  mat * m = (mat *) malloc(sizeof(mat));
  assert(m != NULL);

  m -> rows = rows;
  m -> cols = cols;

  m -> cells = (double **)malloc(sizeof(double *) * rows);
  assert(m -> cells != NULL);

  int i;
  for (i = 0; i < rows; i++){
    m -> cells[i] = (double *)malloc(sizeof(double) * cols);
    assert(m -> cells[i] != NULL);
    bzero(m -> cells[i],sizeof(double) * cols);
  }

  return m;
}

// Constructor.  Create random matrix.
mat * create_mat_random(int rows, int cols, int max){
  
  mat * m = create_mat_zero(rows, cols);

  int i,j;
  for (i = 0; i < rows; i++){
    for (j = 0; j < cols; j++){
      m -> cells[i][j] = (double)(rand() % max);
    }
  }  

  return m;

}

// Destructor.
void destroy_mat(mat * m) {

  int rows = m -> rows;
  int cols = m -> cols;

  int i;
  for (i = 0; i < rows; i++){
    free(m -> cells[i]);
  }
  
  free(m -> cells);
  free(m);

}

// Displays a matrix.
void print_mat(mat * m) {

  int rows = m -> rows;
  int cols = m -> cols;

  //printf("rows = %d, cols = %d\n", rows, cols);

  int i,j;
  for (i = 0; i < rows; i++){
    for (j = 0; j < cols; j++){
      printf("%8.2f ",m -> cells[i][j]);
    }
    printf("\n");
  }

}

// Multiplies A, B using the naive O(n^3) algorithm.
mat * multiply_mat_naive(mat * A, mat * B) {

  assert(A -> cols == B -> rows);

  mat * C = create_mat_zero(A -> rows, B -> cols);

  int i,j,k;
  for (i = 0; i < A -> rows; i++){

    for (k = 0; k < B -> cols; k++){
    
      for (j = 0; j < A -> cols; j++){
	C -> cells[i][k] += A -> cells[i][j] * B -> cells[j][k];
      }
    }
  }

  return C;

}

// Multiplies A, B using a USP.
mat * multiply_mat_puzzle(mat * A, mat * B, puzzle * p);



