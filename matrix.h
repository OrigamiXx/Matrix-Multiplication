#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "CheckUSP.h"

typedef struct _mat {

  int rows;
  int cols;
  double ** cells;

} mat;


// Constructor. Create a zero matrix.
mat * create_mat_zero(int rows, int cols);

// Constructor.  Create random matrix.
mat * create_mat_random(int rows, int cols, int max);

// Destructor.
void destroy_mat(mat * m);

// Displays a matrix.
void print_mat(mat * m);

// Multiplies A, B using the naive O(n^3) algorithm.
mat * multiply_mat_naive(mat * A, mat * B);

// Multiplies A, B using a USP.
mat * multiply_mat_puzzle(mat * A, mat * B, puzzle * p);


#endif
