#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "matrix.h"
#include "permutation.h"
#include "usp.h"
#include "groups.h"
#include "constants.h"
#include "puzzle.h"


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

// Copy constructor.
mat * copy_mat(mat * m1){

  int rows = m1 -> rows;
  int cols = m1 -> cols;

  mat * m2 = create_mat_zero(rows, cols);

  int i,j;
  for (i = 0; i < rows; i++){
    for (j = 0; j < cols; j++){
      m2 -> cells[i][j] = m1 -> cells[i][j];
    }
  }  

  return m2;

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

// Converts a matrix into an element of K[G] using basis.
elt_KG * mat_to_elt_KG(mat * m, elt_KG * s, elt_KG * t){

  //printf("rows = %d, s->size = %d\n",m -> rows, s -> size);
  assert(m -> rows <= s -> size);
  assert(m -> cols <= t -> size);

  elt_KG * r = create_elt_KG_identity_zero(s -> U, s -> k, s -> m);
  
  int i, j;
  int rows = m -> rows;
  int cols = m -> cols;
  
  int ix = 0;
  int jx = 0;

  for (i = 0; i < rows; i++){

    while (s -> elements -> entries[ix].flag != HASH_OCCUPIED){
      ix++;
    }

    elt_G * g1 = (elt_G *)(s -> elements -> entries[ix].key);

    jx = 0;

    for (j = 0; j < cols; j++){

      while (t -> elements -> entries[jx].flag != HASH_OCCUPIED){
	jx++;
      }

      elt_G * g2 = (elt_G *)(t -> elements -> entries[jx].key);

      elt_G * g = inverse_elt_G_new(g1);
      multiply_elt_G(g, g2);

      add_basis_elt_KG(r,g,m -> cells[i][j]);

      destroy_elt_G(g);

      jx++;
    }

    ix++;
  }

  return r;

}

// Converts an element of K[G] into a matrix using basis.
mat * elt_KG_to_mat(elt_KG * r, elt_KG * s, elt_KG * t, int rows, int cols) {

  //int rows = s -> size;
  //int cols = t -> size;

  mat * m = create_mat_zero(rows,cols);
    
  int i, j;

  int ix = 0;
  int jx = 0;

  for (i = 0; i < rows; i++){

    while (s -> elements -> entries[ix].flag != HASH_OCCUPIED){
      ix++;
    }

    elt_G * g1 = (elt_G *)(s -> elements -> entries[ix].key);

    jx = 0;

    for (j = 0; j < cols; j++){

      while (t -> elements -> entries[jx].flag != HASH_OCCUPIED){
	jx++;
      }

      elt_G * g2 = (elt_G *)(t -> elements -> entries[jx].key);

      elt_G * g = inverse_elt_G_new(g1);
      multiply_elt_G(g, g2);

      m -> cells[i][j] = get_coef_elt_KG(r,g);

      destroy_elt_G(g);

      jx++;
    }
    ix++;
  }

  return m;

}




// Multiplies A, B using a USP.
mat * multiply_mat_puzzle(mat * A, mat * B, puzzle * p, int m) {

  assert(A -> cols == B -> rows);

  elt_KG * s1;
  elt_KG * s2;
  elt_KG * s3;

  int n1 = A -> rows;
  int n2 = A -> cols;
  int n3 = B -> cols;

  create_Sis(p, m, &s1, &s2, &s3,n1,n2,n3);

  //print_compact_elt_KG(s1);
  //print_compact_elt_KG(s2);
  //print_compact_elt_KG(s3);

  DEBUG3_PRINTF("\rRealizing <%d,%d,%d>\n",s1 -> size, s2 -> size, s3 -> size);

  elt_KG * a = mat_to_elt_KG(A,s1,s2);
  elt_KG * b = mat_to_elt_KG(B,s2,s3);
  
  /*
  printf("a = \n");
  print_compact_elt_KG(a);
  printf("b = \n");
  print_compact_elt_KG(b);
  //*/

  printf("Starting to multiply.\n");

  elt_KG * c = multiply_elt_KG_new(a,b);

  /*
  printf("c = \n");
  print_compact_elt_KG(c);
  //*/

  mat * C = elt_KG_to_mat(c,s1,s3,n1,n3);

  destroy_elt_KG(s1);
  destroy_elt_KG(s2);
  destroy_elt_KG(s3);
  destroy_elt_KG(a);
  destroy_elt_KG(b);
  destroy_elt_KG(c);

  return C;
}

// Multiplies A, B using sets derived from a USP.
mat * multiply_mat_sets(mat * A, mat * B, elt_KG * s1, elt_KG * s2, elt_KG * s3){

  elt_KG * a = mat_to_elt_KG(A,s1,s2);
  elt_KG * b = mat_to_elt_KG(B,s2,s3);

  /*
  printf("a = \n");
  print_compact_elt_KG(a);
  printf("b = \n");
  print_compact_elt_KG(b);
  */

  elt_KG * c = multiply_elt_KG_new(a,b);

  /*
  printf("c = \n");
  print_compact_elt_KG(c);
  */

  mat * C = elt_KG_to_mat(c,s1,s3,A -> rows,B -> cols);

  destroy_elt_KG(a);
  destroy_elt_KG(b);
  destroy_elt_KG(c);

  return C;

}


// A += B
void add_mat(mat * A, mat * B) {

  assert(A -> rows == B -> rows);
  assert(A -> cols == B -> cols);

  int rows = A -> rows;
  int cols = A -> cols;

  int i,j;
  for (i = 0; i < rows; i++){
    for (j = 0; j < cols; j++){
      A -> cells[i][j] += B -> cells[i][j];
    }
  }
}

// C = A + B
mat * add_mat_new(mat * A, mat * B) {

  mat * C = copy_mat(A);

  add_mat(C,B);

  return C;

}

// A *= c
void scalar_multiply_mat(mat * A, double c){

  int rows = A -> rows;
  int cols = A -> cols;

  int i,j;
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++)
      A -> cells[i][j] *= c;

}


// B = A * c
mat * scalar_multiply_mat_new(mat * A, double c) {

  mat * B = copy_mat(A);

  scalar_multiply_mat(B,c);

  return B;

}

double one_norm_mat(mat * A){

  int rows = A -> rows;
  int cols = A -> cols;

  double norm = 0.0;

  int i,j;
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++)
      norm += fabs(A -> cells[i][j]);
  
  return norm;

}



// Returns true iff p is a strong USP.  Warning: Has a small chance of
// saying true when p is not a strong USP.
int check_usp_mult(puzzle * p){ 

  time_t t;
  srand((unsigned) time(&t));

  elt_KG * s1;
  elt_KG * s2;
  elt_KG * s3;

  int m = 2; // XXX - may not be sufficient.

  create_Sis(p, m, &s1, &s2, &s3,-1,-1,-1);

  int n1 = s1 -> size;
  int n2 = s2 -> size;
  int n3 = s3 -> size;

  /*
  printf("s1 = \n");
  print_elt_KG(s1);
  printf("s2 = \n");
  print_elt_KG(s2);
  printf("s3 = \n");
  print_elt_KG(s3);
  */

  int max = 10;
    
  mat * m1 = create_mat_random(n1, n2, max);
  mat * m2 = create_mat_random(n2, n3, max);

  mat * m3 = multiply_mat_naive(m1,m2);
  mat * m4 = multiply_mat_sets(m1,m2,s1,s2,s3);

  /* printf("m1 = \n"); */
  /* print_mat(m1); */

  /* printf("m2 = \n"); */
  /* print_mat(m2); */

  /* printf("m1 * m2 = \n"); */
  /* print_mat(m3); */

  /* printf("m4 = \n"); */
  /* print_mat(m4); */

  scalar_multiply_mat(m4,-1.0);
  add_mat(m4,m3);

  double dist = one_norm_mat(m4);

  destroy_mat(m1);
  destroy_mat(m2);
  destroy_mat(m3);
  destroy_mat(m4);

  if (dist < 1.0)
    return true;
  else
    return false;

}
