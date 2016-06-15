
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
mat * create_mat_zero(int rows, int cols);

// Destructor.
void destroy_mat(mat * m);

// Displays a matrix.
void print_mat(mat * m);

// Multiplies A, B using the naive O(n^3) algorithm.
mat * multiply_naive(mat * A, mat * B);

// Multiplies A, B using a USP.
mat * multiply_puzzle(mat * A, mat * B, puzzle * p);



