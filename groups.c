// The purpose of this file is to implement the groups based on USPs
// from Section 3.2 of [CKSU'05].  They serve as the basic
// datastructures necessary to implement the matrix multiplication
// algorithm.

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include "permutation.h"
#include "groups.h"
#include "constants.h"



// ================================================================================
//
//  Implementation of Group H
//
// ================================================================================

// Default constructor.  Allocates empty elt_H structure, function is identity.
elt_H * create_elt_H_identity(int U, int k, int m){

  elt_H * h = (elt_H *) malloc(sizeof(elt_H));
  assert(h != NULL);

  h -> U = U;
  h -> k = k;
  h -> m = m;

  h -> f = (int **)malloc(sizeof(int *) * (h -> U));
  assert(h -> f != NULL);

  int i;
  for (i = 0; i < h -> U; i++){
    h -> f[i] = (int *)malloc(sizeof(int) * (h -> k));
    assert(h -> f[i] != NULL);
    bzero(h->f[i],sizeof(int)*(h->k));
  }

  return h;

}

// Random constructor.  Allocates empty elt_H structure, function is random.
// XXX - Need to set seed before using this for non-testing purposes.
elt_H * create_elt_H_random(int U, int k, int m){

  elt_H * h = create_elt_H_identity(U,k,m);

  int i,j;
  for (i = 0; i < U; i++){
    for (j = 0; j < k; j++){
      h -> f[i][j] =  rand() % m;
    }
  }
  
  return h;
}


// Copy constructor.
elt_H * copy_elt_H(elt_H * h1){

  int U = h1 -> U;
  int k = h1 -> k;
  int m = h1 -> m;

  elt_H * h2 = create_elt_H_identity(U,k,m);

  int i,j;
  for (i = 0; i < U; i++){
    memcpy(h2 -> f[i],h1->f[i],sizeof(int)*k);
  }

  return h2;
}

// Destructor.
void destroy_elt_H(elt_H * h){

  int U = h -> U;
  int k = h -> k;

  int i;
  for (i = 0; i < U; i++){
    free(h -> f[i]);
  }
  
  free(h -> f);
  free(h);

}

// Returns true iff h = 1.
int is_identity_elt_H(elt_H * h){

  int U = h -> U;
  int k = h -> k;

  int i,j;
  for (i = 0; i < U; i++){
    for (j = 0; j < k; j++){
      if (h -> f[i][j] != 0)
	return false;
    }
  }

  return true;

}

// Returns true iff h1 = h2.
int equals_elt_H(elt_H * h1, elt_H * h2){

  if (h1 -> U != h2 -> U)
    return false;

  if (h1 -> k != h2 -> k)
    return false;

  if (h1 -> m != h2 -> m)
    return false;

  int U = h1 -> U;
  int k = h1 -> k;

  int i,j;
  for (i = 0; i < U; i++){
    for (j = 0; j < k; j++){
      if (h1 -> f[i][j] != h2 -> f[i][j])
	return false;
    }
  }

  return true;

}

// Addition operator.  Returns new copy.
elt_H * add_elt_H_new(elt_H * h1, elt_H * h2){

  elt_H * h = copy_elt_H(h1);

  add_elt_H(h,h2);

  return h;

}

// Addition operator.  Adds to first parameter.  No allocation.
void add_elt_H(elt_H * h1, elt_H * h2){

  assert(h1 -> U == h2 -> U);
  assert(h1 -> k == h2 -> k);
  assert(h1 -> m == h2 -> m);
  
  int U = h1 -> U;
  int k = h1 -> k;
  int m = h1 -> m;

  int i,j;
  for (i = 0; i < U; i++){
    for (j = 0; j < k; j++){
      h1 -> f[i][j] =  (h1 -> f[i][j] + h2 -> f[i][j]) % m;
    }
  }

}

// Inverts an element of h.  No allocation.
void inverse_elt_H(elt_H * h){

  int U = h -> U;
  int k = h -> k;
  int m = h -> m;

  int i,j;
  for (i = 0; i < U; i++){
    for (j = 0; j < k; j++){
      h -> f[i][j] =  (m - (h -> f[i][j])) % m;
    }
  }


}
 
// Return inverse of parameter as new copy.  
elt_H * inverse_elt_H_new(elt_H * h1){

  elt_H * h2 = copy_elt_H(h1);

  inverse_elt_H(h2);

  return h2;

}

// Applies permutation action to element of H.  No allocation.
void apply_elt_H(elt_H * h, permutation * pi){

  int U = h -> U;

  assert(U <= 30);

  int * tmp[30]; // XXX could allocate dynamically, using = (int **) malloc(sizeof(int *) * 30);
  memcpy(tmp, h -> f, sizeof(int *) * U);

  int i;

  for (i = 0; i < U; i++){
    h -> f[i] = tmp[Apply_permutation(pi,i)];
  }

  //free(tmp);

}

// Applies permutation action to element of H.  Returns new copy.
elt_H * apply_elt_H_new(elt_H * h1, permutation * pi){

  elt_H * h2 = copy_elt_H(h1);

  apply_elt_H(h2,pi);

  return h2;

}

// Display an element of H.
void print_elt_H(elt_H * h){
  
  int U = h -> U;
  int k = h -> k;
  int m = h -> m;

  printf("[U: %d, k: %d, m: %d,\n",U,k,m);

  int i,j;
  for (i = 0; i < U; i++){
    for (j = 0; j < k; j++){
      printf("%d ",h -> f[i][j]);
    }
    printf("\n");
  }
  printf("]\n");

}

// In place iterator for elements of H.  No allocation.
// XXX - todo.
void next_elt(elt_H * h);

// Iterator for elements of H. Returns new copy.
// XXX - todo.
void next_elt(elt_H * h);


// ================================================================================
//
//  Implementation of Group G
//
// ================================================================================


// Default constructor.  Allocates new elt_G structure, doesn't copy h or p.
elt_G * create_elt_G(elt_H * h, permutation *pi){

  assert(h -> U == pi -> size);

  elt_G * g = (elt_G *) malloc(sizeof(elt_G));

  assert(g != NULL);

  g -> h = h;
  g -> pi = pi;

  return g;
}

// Default constructor.  Allocates new elt_G structure, copies h and p.
elt_G * create_elt_G_new(elt_H * h, permutation *pi){
  
  return create_elt_G(copy_elt_H(h),copy_permutation(pi));

}

// Copy constructor.
elt_G * copy_elt_G(elt_G * g){

  return create_elt_G_new(g -> h, g -> pi);

}

// Destructor.  Deallocates everything.  XXX - Perhaps implements a shallow destructor.
void destroy_elt_G(elt_G * g) {

  destroy_elt_H(g -> h);
  destroy_perm(g -> pi);
  free(g);

}

// Returns true if g is the identity in G.
int is_identity_elt_G(elt_G * g){
  
  return is_identity_elt_H(g -> h) && is_identity_permutation(g -> pi); 

}

// Returns true iff g1 = g2.
int equals_elt_G(elt_G * g1, elt_G * g2){

  return equals_elt_H(g1 -> h, g2 -> h) && equals(g1 -> pi, g2 -> pi);

}

// Multiplies two elements of G and returns a new copy.
elt_G * multiply_elt_G_new(elt_G * g1, elt_G *g2){

  elt_G * g = copy_elt_G(g1);

  multiply_elt_G(g,g2);

  return g;

}

// Multiplies two elements of G, replacing the first parameter.
void multiply_elt_G(elt_G * g1, elt_G *g2){

  // Semi-direct product: (h1,pi1).(h2,pi2) = (h1^pi2 h2, pi1 pi2)

  apply_elt_H(g1 -> h, g2 -> pi);
  add_elt_H(g1 -> h, g2 -> h);

  // Compose allocates a new permutation, so we need to free the original.
  permutation * pi = g1 -> pi;
  g1 -> pi = compose(g1 -> pi, g2 -> pi); 
  destroy_perm(pi);
 
}

// Inverts an element of h.  No allocation.
void inverse_elt_G(elt_G * g) {

  // Inverse of (h,pi) is ((h^-1)^(pi^-1), pi^-1).
  inverse_elt_H(g -> h);
  g -> pi = inverse_permutation(g -> pi);
  apply_elt_H(g -> h,g -> pi);

}
  

// Return inverse of parameter as new copy.  
elt_G * inverse_elt_G_new(elt_G * g1){

  elt_G * g2 = copy_elt_G(g1);

  inverse_elt_G(g2);

  return g2;

}

// Displays given element of G.
void print_elt_G(elt_G * g){

  printf("(h = \n");
  print_elt_H(g -> h);
  printf("---\n");
  print(g -> pi);
  printf(")\n");
    
}

// Returns an array containing all elements of G satisfying hp(u,j) =
// 0 iff u_j = i for all u in U, j in [k].  Length is set to the
// length of this array.
// XXX - Todo.
elt_G ** create_Sis(puzzle * p, int i, int * length);




// ================================================================================
//
//  Implementation of Group Algebra K[G]
//
// ================================================================================


// XXX - Todo.
