
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include "permutation.h"
#include "groups.h"
#include "constants.h"


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

// Returns true if h is identity and false otherwise.
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
void apply_elt_H(elt_H * h, permutation * p){

  int U = h -> U;

  assert(U <= 30);

  int * tmp[30]; // XXX could allocate dynamically, using = (int **) malloc(sizeof(int *) * 30);
  memcpy(tmp, h -> f, sizeof(int *) * U);

  int i;

  for (i = 0; i < U; i++){
    h -> f[i] = tmp[Apply_permutation(p,i)];
  }

  //free(tmp);

}

// Applies permutation action to element of H.  Returns new copy.
elt_H * apply_elt_H_new(elt_H * h1, permutation * p){

  elt_H * h2 = copy_elt_H(h1);

  apply_elt_H(h2,p);

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
void next_elt(elt_H * h);

// Iterator for elements of H. Returns new copy.
void next_elt(elt_H * h);
