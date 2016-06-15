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


// Constructor.  Does not copy g.
basis_elt_KG * create_basis_elt_KG(elt_G * g, double c){

  basis_elt_KG * r = (basis_elt_KG *) malloc(sizeof(basis_elt_KG));
  assert(r != NULL);

  r -> g = g;
  r -> c = c;
  r -> next = NULL;

  return r;

}

// Constructor.  Copies the element of g.
basis_elt_KG * create_basis_elt_KG_new(elt_G * g, double c){

  basis_elt_KG * r = create_basis_elt_KG(g,c);

  r -> g = copy_elt_G(g);

  return r;

}

// Deep Copy constructor. 
basis_elt_KG * copy_basis_elt_KG(basis_elt_KG * r){

  return create_basis_elt_KG_new(r -> g, r -> c);

}

// Destructor.
void destroy_basis_elt_KG(basis_elt_KG * r){

  destroy_elt_G(r -> g);
  free(r);

}

// Displays basis element of K[G].
void print_basis_elt_KG(basis_elt_KG * r) {

  printf("{\n");
  print_elt_G(r -> g);
  printf("===\n");
  printf("%f\n}\n", r -> c);

}


// Constructor.  Create additive identity in K[G].
elt_KG * create_elt_KG_identity_zero(int U, int k, int m) {
  
  elt_KG * r = (elt_KG *) malloc(sizeof(elt_KG));
  assert(r != NULL);

  r -> U = U;
  r -> k = k;
  r -> m = m;
  r -> head = NULL;
  r -> size = 0;

  return r;

}

// Constructor. Create multiplicative idenity in K[G].
elt_KG * create_elt_KG_identity_one(int U, int k, int m){
  
  elt_KG * r = (elt_KG *) malloc(sizeof(elt_KG));
  assert(r != NULL);

  r -> U = U;
  r -> k = k;
  r -> m = m;
  r -> head = create_basis_elt_KG(create_elt_G(create_elt_H_identity(U,k,m),ID_permutation(U)),1);
  r -> size = 1;

  return r;

}

// Copy constructor. Create a copy of an element of K[G].
elt_KG * copy_elt_KG(elt_KG * r1) {

  elt_KG * r2 = create_elt_KG_identity_zero(r1 -> U, r1 -> k, r1 -> m);
  
  r2 -> size = r1 -> size;
  int i = 0;
  basis_elt_KG ** prev_ptr = &r2 -> head;
  basis_elt_KG * curr = r1 -> head;
  for (i = 0; i < r1 -> size; i++) {

    *prev_ptr = copy_basis_elt_KG(curr);
    prev_ptr = &((*prev_ptr) -> next);
    curr = curr -> next;

  }

  return r2;

}

// Destructor.
void destroy_elt_KG(elt_KG * r) {

  basis_elt_KG * curr = r -> head;

  while (curr != NULL){

    basis_elt_KG * tmp = curr;
    curr = curr -> next;
    destroy_basis_elt_KG(tmp);
    
  }

  free(r);

}

// Returns a pointer to the basis element matching g, NULL if not found.
basis_elt_KG * locate_basis_elt_KG(elt_KG * r, elt_G * g) {

  basis_elt_KG * curr = r -> head;
  int i;
  for (i = 0; i < r -> size; i++) 
    if (equals_elt_G(curr -> g, g))
      return curr;

  return NULL;

}

// Adds c to the coefficient of g in r.  
void add_basis_elt_KG(elt_KG * r, elt_G * g, double c) {

  basis_elt_KG * e = locate_basis_elt_KG(r,g);

  if (e == NULL) {
    e = create_basis_elt_KG_new(g, c);
    e -> next = r -> head;
    r -> head = e;
    r -> size++;
  } else {
    e -> c += c;
  }

}

// Adds r1 to r2, replaces r1.
void add_elt_KG(elt_KG * r1, elt_KG * r2) {

  basis_elt_KG * curr = r2 -> head;
  int i;
  for (i = 0; i < r2 -> size; i++) {
    add_basis_elt_KG(r1, curr -> g, curr -> c);
    curr = curr -> next;
  }

}

// Adds r2 to r1, returns new copy of result.
elt_KG * add_elt_KG_new(elt_KG * r1, elt_KG * r2) {

  elt_KG * r = copy_elt_KG(r1);

  add_elt_KG(r,r2);

  return r;

}


// Multiples r1 and r2, returns new copy of result.
elt_KG * multiply_elt_KG_new(elt_KG * r1, elt_KG * r2) {

  elt_KG * r = create_elt_KG_identity_zero(r1 -> U, r1 -> k, r1 -> m);

  basis_elt_KG * curr1 = r1 -> head;
  int i,j;

  for (i = 0; i < r1 -> size; i++){

    basis_elt_KG * curr2 = r2 -> head;

    for (j = 0; j < r2 -> size; j++){

      elt_G * g = multiply_elt_G_new(curr1 -> g, curr2 -> g);
      double c = curr1 -> c * curr2 -> c;

      add_basis_elt_KG(r,g,c);

      destroy_elt_G(g);
      
      curr2 = curr2 -> next;
    }

    curr1 = curr1 -> next;
  }

  return r;

}


// Multiples r by c, replaces r.
void scalar_multiply_elt_KG(elt_KG * r, double c) {

  basis_elt_KG * curr = r -> head;
  int i;
  for (i = 0; i < r -> size; i++) {
    curr -> c *= c;
    curr = curr -> next;
  }

}

// Multiples r by c, returns new copy of result.
elt_KG * scalar_multiply_elt_KG_new(elt_KG * r1, double c) {

  elt_KG * r2 = copy_elt_KG(r1);

  scalar_multiply_elt_KG(r2,c);

  return r2;

}

// Returns the coef in K of g in r.
double get_coef_elt_KG(elt_KG * r, elt_G * g) {

  basis_elt_KG * e = locate_basis_elt_KG(r,g);

  if (e == NULL) 
    return 0.0;
  else
    return e -> c;

}

// Displays an element of K[G].
void print_elt_KG(elt_KG * r) {

  printf("<|\n");

  basis_elt_KG * curr = r -> head;
  int i;
  for (i = 0; i < r -> size; i++) {
    print_basis_elt_KG(curr);
    curr = curr -> next;
  }

  printf("|>\n");

}
