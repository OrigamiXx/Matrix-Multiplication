#ifndef __GROUPS_H__
#define __GROUPS_H__

#include "permutation.h"
#include "usp.h"
#include "hash_table.h"

// ================================================================================
//
//  Functions for group H = U x [k] -> C_m
//
// ================================================================================

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

// Returns true iff h1 = h2.
int is_equals_elt_H(elt_H * h1, elt_H * h2);

// Addition operator.  Returns new copy.
elt_H * add_elt_H_new(elt_H * h1, elt_H * h2);

// Addition operator.  Adds to first parameter.  No allocation.
void add_elt_H(elt_H * h1, elt_H * h2);

// Inverts an element of h.  No allocation.
void inverse_elt_H(elt_H * h); 
 
// Return inverse of parameter as new copy.  
elt_H * inverse_elt_H_new(elt_H * h);

// Applies permutation action to element of H.  No allocation.
void apply_elt_H(elt_H * h, perm * pi);

// Applies permutation action to element of H.  Returns new copy.
elt_H * apply_elt_H_new(elt_H * h, perm * pi);

// Display an element of H.
void print_elt_H(elt_H * h);

// Compact display an element of H.
void print_compact_elt_H(elt_H * h);

// In place iterator for elements of H.  No allocation.
void next_elt(elt_H * h);

// Iterator for elements of H. Returns new copy.
void next_elt(elt_H * h);


// ================================================================================
//
//  Functions for group G = H x< Sym_U
//
// ================================================================================


typedef struct _elt_G{

  elt_H * h;
  perm * pi;
  unsigned int hash;

} elt_G;


// Default constructor.  Allocates new elt_G structure, doesn't copy h or p.
elt_G * create_elt_G(elt_H * h, perm *pi);

// Default constructor.  Allocates new elt_G structure, copies h and p.
elt_G * create_elt_G_new(elt_H * h, perm *pi);

// Copy constructor.
elt_G * copy_elt_G(elt_G * g);

// Destructor.
void destroy_elt_G(elt_G * g);

// Returns true if g is the identity in G.
int is_identity_elt_G(elt_G * g);

// Returns true iff g1 = g2.
int is_equals_elt_G(elt_G * g1, elt_G * g2);

// Multiplies two elements of G and returns a new copy.
elt_G * multiply_elt_G_new(elt_G * g1, elt_G *g2);

// Multiplies two elements of G, replacing the first parameter.
void multiply_elt_G(elt_G * g1, elt_G *g2);

// Inverts an element of h.  No allocation.
void inverse_elt_G(elt_G * g);

// Return inverse of parameter as new copy.  
elt_G * inverse_elt_G_new(elt_G * g);

// Displays given element of G.
void print_elt_G(elt_G * g);

// Compact display given element of G.
void print_compact_elt_G(elt_G * g);




// ================================================================================
//
//  Functions for Group Algebra K[G]  K = R
//
// ================================================================================

// Shoddy implementation as a linked list.  Would perform better as a hash table. 
// Will implement interface to make that transformation as simple as possible.

/* typedef struct _basis_elt_KG{ */

/*   elt_G * g; */
/*   double c; */
/*   struct _basis_elt_KG * next; */

/* } basis_elt_KG; */

/* // Constructor.  Does not copy g. */
/* basis_elt_KG * create_basis_elt_KG(elt_G * g, double c); */

/* // Constructor.  Copies the element of g. */
/* basis_elt_KG * create_basis_elt_KG_new(elt_G * g, double c); */

/* // Deep Copy constructor.  */
/* basis_elt_KG * copy_basis_elt_KG(basis_elt_KG * r); */

/* // Destructor. */
/* void destroy_basis_elt_KG(basis_elt_KG * r); */

/* // Displays basis element of K[G]. */
/* void print_basis_elt_KG(basis_elt_KG * r); */

/* // Compact display of basis element of K[G]. */
/* void print_compact_basis_elt_KG(basis_elt_KG * r); */

typedef struct _elt_KG {

  int U; 
  int k;
  int m;
  int size;

  hash_table * elements;

} elt_KG;

// Constructor.  Create additive identity in K[G].
elt_KG * create_elt_KG_identity_zero(int U, int k, int m);

// Constructor. Create multiplicative idenity in K[G].
elt_KG * create_elt_KG_identity_one(int U, int k, int m);

// Copy constructor. Create a copy of an element of K[G].
elt_KG * copy_elt_KG(elt_KG * r);

// Destructor.
void destroy_elt_KG(elt_KG * r);

// Returns a pointer to the basis element matching g, NULL if not found.
double * locate_basis_elt_KG(elt_KG * r, elt_G * g);

// Adds c to the coefficient of g in r.  
void add_basis_elt_KG(elt_KG * r, elt_G * g, double c);

// Adds r1 to r2, replaces r1.
void add_elt_KG(elt_KG * r1, elt_KG * r2);

// Adds r2 to r1, returns new copy of result.
elt_KG * add_elt_KG_new(elt_KG * r1, elt_KG * r2);

// Multiples r1 and r2, replaces r1.  XXX - Hard to implement in place.
//void multiply_elt_KG(elt_KG * r1, elt_KG * r2);

// Multiples r1 and r2, returns new copy of result.
elt_KG * multiply_elt_KG_new(elt_KG * r1, elt_KG * r2);

// Multiples r by c, replaces r.
void scalar_multiply_elt_KG(elt_KG * r, double c);

// Multiples r by c, returns new copy of result.
elt_KG * scalar_multiply_elt_KG_new(elt_KG * r, double c);

// Returns the coef in K of g in r.
double get_coef_elt_KG(elt_KG * r, elt_G * g);

// Displays an element of K[G].
void print_elt_KG(elt_KG * r);

// Compact Display of an element of K[G].
void print_compact_elt_KG(elt_KG * r);

// Checks whether g in S_i.
int is_valid_elt_G(elt_G * g, puzzle * p, int i);

// Converts an long long into an element of H.
elt_H * ll_to_elt_H(long long x, int U, int k, int m);

// Returns an array containing all elements of G satisfying hp(u,j) =
// 0 iff u_j = i for all u in U, j in [k].  Length is set to the
// length of this array.
void create_Sis(puzzle * p, int m, elt_KG ** s1_ptr, elt_KG ** s2_ptr, elt_KG ** s3_ptr, int n1, int n2, int n3);

#endif
