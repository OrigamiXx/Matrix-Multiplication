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

  assert(m -> rows <= s -> size);
  assert(m -> cols <= t -> size);

  elt_KG * r = create_elt_KG_identity_zero(s -> U, s -> k, s -> m);
  
  int i, j;
  int rows = m -> rows;
  int cols = m -> cols;
  
  basis_elt_KG * s_curr = s -> head;

  for (i = 0; i < rows; i++){

    basis_elt_KG * t_curr = t -> head;

    for (j = 0; j < cols; j++){

      elt_G * g = inverse_elt_G_new(s_curr -> g);
      multiply_elt_G(g, t_curr -> g);

      add_basis_elt_KG(r,g,m -> cells[i][j]);

      destroy_elt_G(g);
      
      t_curr = t_curr -> next;
    }

    s_curr = s_curr -> next;
  }

  return r;

}

// Converts an element of K[G] into a matrix using basis.
mat * elt_KG_to_mat(elt_KG * r, elt_KG * s, elt_KG * t) {

  int rows = s -> size;
  int cols = t -> size;

  mat * m = create_mat_zero(rows,cols);
    
  int i, j;
  
  basis_elt_KG * s_curr = s -> head;

  for (i = 0; i < rows; i++){

    basis_elt_KG * t_curr = t -> head;

    for (j = 0; j < cols; j++){

      elt_G * g = inverse_elt_G_new(s_curr -> g);
      multiply_elt_G(g, t_curr -> g);

      m -> cells[i][j] = get_coef_elt_KG(r,g);

      destroy_elt_G(g);
      
      t_curr = t_curr -> next;
    }

    s_curr = s_curr -> next;
  }

  return m;

}




// Multiplies A, B using a USP.
mat * multiply_mat_puzzle(mat * A, mat * B, puzzle * p) {

  elt_KG * s1;
  elt_KG * s2;
  elt_KG * s3;

  int m = 3; // XXX - Fix.

  create_Sis(p, m, &s1, &s2, &s3);

  elt_KG * a = mat_to_elt_KG(A,s1,s2);
  elt_KG * b = mat_to_elt_KG(B,s2,s3);
  
  elt_KG * c = multiply_elt_KG_new(a,b);

  mat * C = elt_KG_to_mat(c,s1,s3);

  destroy_elt_KG(s1);
  destroy_elt_KG(s2);
  destroy_elt_KG(s3);
  destroy_elt_KG(a);
  destroy_elt_KG(b);
  destroy_elt_KG(c);

  return C;
}



