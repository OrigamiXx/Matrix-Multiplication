#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "CheckUSP.h"
#include "groups.h"

typedef struct _mat {

  int rows;
  int cols;
  double ** cells;

} mat;


// Constructor. Create a zero matrix.
mat * create_mat_zero(int rows, int cols);

// Constructor.  Create random matrix.
mat * create_mat_random(int rows, int cols, int max);

// Copy constructor.
mat * copy_mat(mat * m);

// Destructor.
void destroy_mat(mat * m);

// Displays a matrix.
void print_mat(mat * m);

// Converts a matrix into an element of K[G] using basis.
elt_KG * mat_to_elt_KG(mat * m, elt_KG * s, elt_KG * t);

// Converts an element of K[G] into a matrix using basis.
mat * elt_KG_to_mat(elt_KG * r, elt_KG * s, elt_KG * t, int rows, int cols);

// Multiplies A, B using the naive O(n^3) algorithm.
mat * multiply_mat_naive(mat * A, mat * B);

// Multiplies A, B using a USP.
mat * multiply_mat_puzzle(mat * A, mat * B, puzzle * p, int m);

// Multiplies A, B using sets derived from a USP.
mat * multiply_mat_sets(mat * A, mat * B, elt_KG * s1, elt_KG * s2, elt_KG *s3);	

// A += B
void add_mat(mat * A, mat * B);

// C = A + B
mat * add_mat_new(mat * A, mat * B);

// A *= c
void scalar_multiply_mat(mat * A, double c);

// B = A * c
mat * scalar_multiply_mat_new(mat * A, double c);

// Return the 1-norm of A.
double one_norm_mat(mat * A);

// Returns true iff p is a strong USP.  Warning: Has a small chance of
// saying true when p is not a strong USP.
int is_usp(puzzle * p);

#endif
