#ifndef __GROUPS_H__
#define __GROUPS_H__

#include "permutation.h"

// Elements of H = U x [k] -> C_m

// Implement as 2D arrays of ints giving the characteristic of the
// function, i.e, (C_m)^{U x [k]}.  Could use puzzle as a base.

typedef struct _elt_H{

  int U; // Size of puzzle
  int k; // Width of puzzle
  int ** f; // Characteristic sequence. 
  int m; // Size of Cyclic group.

} elt_H;


// Default constructor.  Allocates empty elt_H structure, function is identity.
elt_H * create_elt_H_identity(int U, int k, int m);

// Random constructor.  Allocates empty elt_H structure, function is random.
elt_H * create_elt_H_random(int U, int k, int m);

// Copy constructor.
elt_H * copy_elt_H(elt_H * h);

// Destructor.
void destroy_elt_H(elt_H * h);

// Returns true if h is the identity in H.
int is_identity_elt_H(elt_H * h);

// Addition operator.  Returns new copy.
elt_H * add_elt_H_new(elt_H * h1, elt_H * h2);

// Addition operator.  Adds to first parameter.  No allocation.
void add_elt_H(elt_H * h1, elt_H * h2);

// Inverts an element of h.  No allocation.
void inverse_elt_H(elt_H * h); 
 
// Return inverse of parameter as new copy.  
elt_H * inverse_elt_H_new(elt_H * h);

// Applies permutation action to element of H.  No allocation.
void apply_elt_H(elt_H * h, permutation * p);

// Applies permutation action to element of H.  Returns new copy.
elt_H * apply_elt_H_new(elt_H * h, permutation * p);

// Display an element of H.
void print_elt_H(elt_H * h);

// In place iterator for elements of H.  No allocation.
void next_elt(elt_H * h);

// Iterator for elements of H. Returns new copy.
void next_elt(elt_H * h);



typedef struct _elt_G{

  elt_H * h;
  permutation * p;

} elt_G;


// Default constructor.  Allocates new elt_G structure, doesn't copy h or p.
elt_G * create_elt_G(elt_H * h, permutation *p);

// Default constructor.  Allocates new elt_G structure, copies h and p.
elt_G * create_elt_G_new(elt_H * h, permutation *p);

// Copy constructor.
elt_G * copy_elt_G(elt_G * g);

// Destructor.
void destroy_elt_G(elt_G * g);

// Multiplies two elements of G and returns a new copy.
elt_G * multiply_elt_G_new(elt_G * g1, elt_G *g2);

// Multiplies two elements of G, replacing the first parameter.
void multiply_elt_G(elt_G * g1, elt_G *g2);

// Inverts an element of h.  No allocation.
void inverse_elt_G(elt_G * g);

// Return inverse of parameter as new copy.  
elt_G * inverse_elt_G_new(elt_G * g);

// Returns an array containing all elements of G satisfying hp(u,j) =
// 0 iff u_j = i for all u in U, j in [k].  Length is set to the
// length of this array.
elt_G ** create_Sis(puzzle * p, int i, int * length);

/*
- Elements of F[G]
    - Implement as a dictionary (hash table underlying?)
    - Op: Multiply (implement naively)
    - Op: Set element
    - Op: Get element
*/

#endif
