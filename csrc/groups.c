// The purpose of this file is to implement the groups based on USPs
// from Section 3.2 of [CKSU'05].  They serve as the basic
// datastructures necessary to implement the matrix multiplication
// algorithm.

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <math.h>
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

  int i;
  for (i = 0; i < U; i++){
    memcpy(h2 -> f[i],h1->f[i],sizeof(int)*k);
  }

  return h2;
}

// Destructor.
void destroy_elt_H(elt_H * h){

  int U = h -> U;
  //int k = h -> k;

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
int is_equals_elt_H(elt_H * h1, elt_H * h2){

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
void apply_elt_H(elt_H * h, perm * pi){

  int U = h -> U;

  assert(U <= 30);

  int * tmp[30]; // XXX could allocate dynamically, using = (int **) malloc(sizeof(int *) * 30);
  memcpy(tmp, h -> f, sizeof(int *) * U);

  int i;

  for (i = 0; i < U; i++){
    //h -> f[i] = tmp[apply_perm(pi,i)];
    h -> f[apply_perm(pi,i)] = tmp[i]; // XXX - not sure which of these is correct.
  }

  //free(tmp);

}

// Applies perm action to element of H.  Returns new copy.
elt_H * apply_elt_H_new(elt_H * h1, perm * pi){

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

// Compact display of an element of H.
void print_compact_elt_H(elt_H * h){

  int U = h -> U;
  int k = h -> k;
  //int m = h -> m;

  //printf("[U: %d, k: %d, m: %d, ",U,k,m);
  printf("[ ");

  int i,j;
  for (i = 0; i < U; i++){
    for (j = 0; j < k; j++){
      printf("%d ",h -> f[i][j]);
      
    }
    if (i != U - 1)
      printf("| ");
  }
  printf("]");

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
elt_G * create_elt_G(elt_H * h, perm *pi){

  assert(h -> U == pi -> n);

  elt_G * g = (elt_G *) malloc(sizeof(elt_G));

  assert(g != NULL);

  g -> h = h;
  g -> pi = pi;
  g -> hash = 0;

  return g;
}

// Default constructor.  Allocates new elt_G structure, copies h and p.
elt_G * create_elt_G_new(elt_H * h, perm *pi){
  
  return create_elt_G(copy_elt_H(h),copy_perm(pi));

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
  
  return is_identity_elt_H(g -> h) && is_identity_perm(g -> pi); 

}

// Returns true iff g1 = g2.
int is_equals_elt_G(elt_G * g1, elt_G * g2){

  return is_equals_elt_H(g1 -> h, g2 -> h) && is_equals_perm(g1 -> pi, g2 -> pi);

}

// Multiplies two elements of G and returns a new copy.
elt_G * multiply_elt_G_new(elt_G * g1, elt_G * g2){

  elt_G * g = copy_elt_G(g1);

  multiply_elt_G(g,g2);

  return g;

}

// Multiplies two elements of G, replacing the first parameter.
void multiply_elt_G(elt_G * g1, elt_G * g2){

  // Semi-direct product: (h1,pi1).(h2,pi2) = (h1^pi2 h2, pi1 pi2)

  apply_elt_H(g1 -> h, g2 -> pi);
  add_elt_H(g1 -> h, g2 -> h);

  // Compose allocates a new perm, so we need to free the original.
  perm * pi = g1 -> pi;
  g1 -> pi = compose_perm(g1 -> pi, g2 -> pi); 
  destroy_perm(pi);
 
}

// Inverts an element of h.  No allocation.
void inverse_elt_G(elt_G * g) {

  // Inverse of (h,pi) is ((h^-1)^(pi^-1), pi^-1).
  inverse_elt_H(g -> h);
  g -> pi = invert_perm(g -> pi);
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
  print_perm_tabular(g -> pi);
  printf(")\n");
    
}

// Displays given element of G.
void print_compact_elt_G(elt_G * g){

  printf("(h = ");
  print_compact_elt_H(g -> h);
  printf(", pi = ");
    print_perm_cycle(g -> pi);
  printf(")");
    
}

// Checks whether g in S_i.
int is_valid_elt_G(elt_G * g, puzzle * p, int i) {

  int U = p -> row;
  int k = p -> column;

  int n,j;


  invert_perm(g -> pi);
  //elt_H * h = g -> h; 
  elt_H * h = apply_elt_H_new(g -> h, g -> pi); // XXX - should this be inverse of pi?

  for (n = 0; n < U; n++){
    for (j = 0; j < k; j++){
      //printf("i = %d, n = %d, j = %d, p[n][j] = %d, h[n][j] = %d\n", i, n, j, p -> puzzle[n][j],h -> f[n][j]);
      if (p -> puzzle[n][j] == i) {
	if (h -> f[n][j] == 0) {
	  invert_perm(g -> pi);
	  destroy_elt_H(h);
	  return false;
	}
      } else {
	if (h -> f[n][j] != 0) {
	  invert_perm(g -> pi);
	  destroy_elt_H(h);
	  return false;
	}
      }

    }
  }

  invert_perm(g -> pi);
  destroy_elt_H(h);
  return true;

}

// Converts an long long into an element of H.
elt_H * ll_to_elt_H(long long x, int U, int k, int m) {

  elt_H * h = create_elt_H_identity(U,k,m);

  int i,j;
  for (i = 0; i < U; i++){
    for (j = 0; j < k; j++){
      h -> f[i][j] = x % m;
      x = x / m;
    }
  }

  return h;

}

// Returns an array containing all elements of G satisfying hp(u,j) =
// 0 iff u_j = i for all u in U, j in [k].   Returned as an element of K[G].
// XXX - Todo.
void create_Sis(puzzle * p, int m, elt_KG ** s1_ptr, elt_KG ** s2_ptr, elt_KG ** s3_ptr, int n1, int n2, int n3) {

  int U = p -> row;
  int k = p -> column;

  *s1_ptr = create_elt_KG_identity_zero(U,k,m);
  *s2_ptr = create_elt_KG_identity_zero(U,k,m);
  *s3_ptr = create_elt_KG_identity_zero(U,k,m);

  long long max = (long long) pow(m, U * k);

  long long x;
  for (x = 0; x < max; x++){

    int size1 = (*s1_ptr) -> size;
    int size2 = (*s2_ptr) -> size;
    int size3 = (*s3_ptr) -> size;

    if (n1 > 0 && n2 > 0 && n3 > 0 && size1 >= n1 && size2 >= n2 && size3 >= n3) {
      break;
    }


    elt_H * h = ll_to_elt_H(x,U,k,m);
    
    perm * pi = create_perm_identity(U);
    if (x % 1000 == 0) {
      //print_compact_elt_H(h);
      DEBUG3_PRINTF("\rRealizing <%d,%d,%d>",(*s1_ptr) -> size, (*s2_ptr) -> size, (*s3_ptr) -> size);
      fflush(stdout);
    }

    while (!is_last_perm(pi)){    

      //print_compact_perm(pi);
      //printf("\n");

      elt_G * g = create_elt_G_new(h,pi);

      if (is_valid_elt_G(g,p,1)) {
	//printf("Added to S_1\n");
	add_basis_elt_KG(*s1_ptr,g,1);

      }

      if (is_valid_elt_G(g,p,2)) {
	//printf("Added to S_2\n");
	add_basis_elt_KG(*s2_ptr,g,1);
      }

      if (is_valid_elt_G(g,p,3)) {
	//printf("Added to S_3\n");
	add_basis_elt_KG(*s3_ptr,g,1);
      }

      destroy_elt_G(g);

      next_perm(pi);
      
    }

    //print_compact_perm(pi);
    //printf("\n");

    elt_G * g = create_elt_G_new(h,pi);
    
    if (is_valid_elt_G(g,p,1)) {
      //printf("Added to S_1\n");
      add_basis_elt_KG(*s1_ptr,g,1);
      
    }
    
    if (is_valid_elt_G(g,p,2)) {
      //printf("Added to S_2\n");
      add_basis_elt_KG(*s2_ptr,g,1);
    }
    
    if (is_valid_elt_G(g,p,3)) {
      //printf("Added to S_3\n");
      add_basis_elt_KG(*s3_ptr,g,1);
      }
    
    destroy_elt_G(g);

    destroy_perm(pi);
    destroy_elt_H(h);

  }

  /*
  elt_G * s = (*s1_ptr)->head->g;
  elt_G * t = (*s2_ptr)->head->g;
  elt_G * u = (*s3_ptr)->head->g;
  elt_G * s_inv = inverse_elt_G_new(s);
  elt_G * t_inv = inverse_elt_G_new(t);
  elt_G * u_inv = inverse_elt_G_new(u);
  */

  /*
  multiply_elt_G(s,s_inv);
  multiply_elt_G(s,t);
  multiply_elt_G(s,t_inv);
  multiply_elt_G(s,u);
  multiply_elt_G(s,u_inv);
  print_compact_elt_G(s);
  */
  
  /*
  elt_G * rhs = multiply_elt_G_new(s_inv,u);
  elt_G * lhs1 = multiply_elt_G_new(s_inv,t);
  elt_G * lhs2 = multiply_elt_G_new(t_inv,u);
  elt_G * lhs = multiply_elt_G_new(lhs1,lhs2);

  elt_G * full = multiply_elt_G_new(s_inv,t);
  multiply_elt_G(full,t_inv);
  multiply_elt_G(full,u);

  printf("lhs1 = \n");
  print_compact_elt_G(lhs1);
  printf("\nlhs2 = \n");
  print_compact_elt_G(lhs2);
  printf("\nCombined\n");
  
  print_compact_elt_G(lhs);
  printf("\n?=\n");
  print_compact_elt_G(rhs);
  printf("\n?=\n");
  print_compact_elt_G(full);
  */
  DEBUG3_PRINTF("\rRealizing <%d,%d,%d>\n",(*s1_ptr) -> size, (*s2_ptr) -> size, (*s3_ptr) -> size);


}

// ================================================================================
//
//  Implementation of Group Algebra K[G]
//
// ================================================================================


/* // Constructor.  Does not copy g. */
/* basis_elt_KG * create_basis_elt_KG(elt_G * g, double c){ */

/*   basis_elt_KG * r = (basis_elt_KG *) malloc(sizeof(basis_elt_KG)); */
/*   assert(r != NULL); */

/*   r -> g = g; */
/*   r -> c = c; */
/*   r -> next = NULL; */

/*   return r; */

/* } */

/* // Constructor.  Copies the element of g. */
/* basis_elt_KG * create_basis_elt_KG_new(elt_G * g, double c){ */

/*   basis_elt_KG * r = create_basis_elt_KG(g,c); */

/*   r -> g = copy_elt_G(g); */

/*   return r; */

/* } */

/* // Deep Copy constructor.  */
/* basis_elt_KG * copy_basis_elt_KG(basis_elt_KG * r){ */

/*   return create_basis_elt_KG_new(r -> g, r -> c); */

/* } */

/* // Destructor. */
/* void destroy_basis_elt_KG(basis_elt_KG * r){ */

/*   destroy_elt_G(r -> g); */
/*   free(r); */

/* } */

/* // Displays basis element of K[G]. */
/* void print_basis_elt_KG(basis_elt_KG * r) { */

/*   printf("{\n"); */
/*   print_elt_G(r -> g); */
/*   printf("===\n"); */
/*   printf("%f\n}\n", r -> c); */

/* } */


/* // Compact display of basis element of K[G]. */
/* void print_compact_basis_elt_KG(basis_elt_KG * r) { */

/*   printf("{c = %f", r -> c); */
/*   printf(", g = "); */
/*   print_compact_elt_G(r -> g); */
/*   printf("}"); */

/* } */

unsigned int hash_elt_G(hash_val k){

  elt_G * g = (elt_G *) k.void_ptr_val;

  if (g -> hash != 0) 
    return g -> hash;

  elt_H * h = g -> h;
  perm * pi = g -> pi;

  long long p_hash = 0;
  int i,j;
  for (i = 0; i < pi -> n; i++){
    p_hash *= 3.0;
    p_hash += pi -> arrow[i];
  }

  long long h_hash = 0;
  
  for (i = 0; i < h -> U; i++){
    for (j = 0; j < h -> k; j++){
      h_hash *= 2.0;
      h_hash += h -> f[i][j];
    }
  }
  
  unsigned int A = 2654435769;

  long long hash = (p_hash % A) * (h_hash % A);

  g -> hash = (unsigned int)(hash % A);

  return g -> hash;

}

int eq_elt_G(hash_val k1, hash_val k2){

  return is_equals_elt_G((elt_G *)k1.void_ptr_val,(elt_G *)k2.void_ptr_val);

}

// Constructor.  Create additive identity in K[G].
elt_KG * create_elt_KG_identity_zero(int U, int k, int m) {
  
  elt_KG * r = (elt_KG *) malloc(sizeof(elt_KG));
  assert(r != NULL);

  r -> U = U;
  r -> k = k;
  r -> m = m;
  r -> elements = create_hash_table(1,hash_elt_G, hash_elt_G,eq_elt_G);
  r -> size = 0;

  return r;

}

// Constructor. Create multiplicative idenity in K[G].
elt_KG * create_elt_KG_identity_one(int U, int k, int m){
  
  elt_KG * r = create_elt_KG_identity_zero(U,k,m);

  add_basis_elt_KG(r,create_elt_G(create_elt_H_identity(U,k,m),create_perm_identity(U)), 1.0);

  return r;

}

hash_val copy_G(hash_val x){

  return (hash_val)((void *) copy_elt_G((elt_G *) (x.void_ptr_val)));

}

void destroy_G(hash_val x){

  destroy_elt_G((elt_G *) x.void_ptr_val);

}

hash_val identity_helper_groups(hash_val x){

  return x;

}

// Copy constructor. Create a copy of an element of K[G].
elt_KG * copy_elt_KG(elt_KG * r1) {

  elt_KG * r2 = create_elt_KG_identity_zero(r1 -> U, r1 -> k, r1 -> m);  
  r2 -> size = r1 -> size;

  destroy_hash_table(r2 -> elements);

  r2 -> elements = copy_hash_table_deep(r1 -> elements, 1.0, copy_G, identity_helper_groups);
  
  //printf("GROUP: end copy KG\n");

  return r2;

}

// Destructor.
void destroy_elt_KG(elt_KG * r) {


  //printf("GROUP: begin destroy KG\n");
  destroy_hash_table_deep(r -> elements,destroy_G,noop_helper);
  free(r);

}

// Returns a pointer to the coef matching g, NULL if not found.
double * locate_basis_elt_KG(elt_KG * r, elt_G * g) {

  hash_val * val_ptr;

  int found = search_in_hash_table(r -> elements,(void *)g,&val_ptr);

  if (found) {
    //printf("GROUP: found %f\n",*val_ptr);
    return &(val_ptr -> d_val);
  } else
    return NULL;

}

// Adds c to the coefficient of g in r.  
void add_basis_elt_KG(elt_KG * r, elt_G * g, double c) {

  
  double * c_ptr = locate_basis_elt_KG(r,g);


  if (c_ptr == NULL) {
    //printf("GROUP: c_ptr = NULL\n");
    
    insert_in_hash_table(r -> elements,(hash_val)(void *)copy_elt_G(g),(hash_val)c);
    r -> size++;
  } else {
    //printf("GROUP: c_ptr != NULL\n");
    //printf("GROUP: in add_basis %f\n",*c_ptr);
    *c_ptr += c;
  }
}

// Adds r1 to r2, replaces r1.
void add_elt_KG(elt_KG * r1, elt_KG * r2) {

  int i;
  for (i = 0; i < r2 -> elements -> capacity; i++) {
    hash_table_entry * e = &(r2 -> elements -> entries[i]);
    if (e -> flag == HASH_OCCUPIED) {

      double c = e -> value.d_val;
      
      //printf("Adding %f\n",c);
      add_basis_elt_KG(r1, (elt_G *)(e -> key.void_ptr_val),  c);
    }
  }

}

// Adds r2 to r1, returns new copy of result.
elt_KG * add_elt_KG_new(elt_KG * r1, elt_KG * r2) {

  //printf("GROUP: Starting add_KG\n");
  
  elt_KG * r = copy_elt_KG(r1);

  add_elt_KG(r,r2);

  return r;

}


// Multiples r1 and r2, returns new copy of result.
elt_KG * multiply_elt_KG_new(elt_KG * r1, elt_KG * r2) {
  
  elt_KG * r = create_elt_KG_identity_zero(r1 -> U, r1 -> k, r1 -> m);
  
  int i, j;
  for (i = 0; i < r1 -> elements -> capacity; i++) {
    hash_table_entry * e1 = &(r1 -> elements -> entries[i]);
    
    if (e1 -> flag == HASH_OCCUPIED) {
      
      elt_G * g1 = (elt_G *)(e1 -> key.void_ptr_val);
      double c1 = e1 -> value.d_val; 
      
      for (j = 0; j < r2 -> elements -> capacity; j++) {
	hash_table_entry * e2 = &(r2 -> elements -> entries[j]);
	//printf("Looping\n");
	
	if (e2 -> flag == HASH_OCCUPIED) {
	  
	  elt_G * g2 = (elt_G *)(e2 -> key.void_ptr_val);
	  double c2 = e2 -> value.d_val;
	  
	  elt_G * g = multiply_elt_G_new(g1, g2);
	  double c = c1 * c2;
	  
	  add_basis_elt_KG(r,g,c); 
	  
	  destroy_elt_G(g);
	  
	}
      }
    }
  }
  
  return r;
}


// Multiples r by c, replaces r.
void scalar_multiply_elt_KG(elt_KG * r, double c) {

  int i;
  for (i = 0; i < r -> elements -> capacity; i++) {
    hash_table_entry * e = &(r -> elements -> entries[i]);
    if (e -> flag == HASH_OCCUPIED) {
      e -> value.d_val *= c;
    }
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

  double * c_ptr = locate_basis_elt_KG(r,g);

  if (c_ptr == NULL) 
    return 0.0;
  else
    return *c_ptr;

}

void print_G(hash_val x){

  print_elt_G((elt_G *)x.void_ptr_val);

}

void print_coef(hash_val x){

  printf("%f",x.d_val);

}

// Displays an element of K[G].
void print_elt_KG(elt_KG * r) {

  printf("<|\n");

  print_hash_table(r -> elements,print_G,print_coef);

  printf("|>\n");

}

// Compact display of an element of K[G].
void print_compact_elt_KG(elt_KG * r) {


  printf("<|\n");

  print_compact_hash_table(r -> elements,print_G,print_coef);

  printf("|>\n");

}
