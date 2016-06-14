#ifndef __GROUPS_H__
#define __GROUPS_H__


// Elements of H = U x [k] -> C_m

// Implement as 2D arrays of ints giving the characteristic of the
// function, i.e, (C_m)^{U x [k]}.  Could use puzzle as a base.

typedef struct _elt_H{

  int U; // Size of puzzle
  int k; // Width of puzzle
  int ** h; // Characteristic sequence. 
  int m; // Size of Cyclic group.

} elt_H;


// Default constructor.  Allocates empty elt_H structure, function is identity.
elt_H * create_elt_H_identity(int U, int k, int m);

// Copy constructor.
elt_H * copy_elt_H(elt_H * h);

// Destructor.
void destroy_elt_H(elt_H * h);

// Addition operator.  Returns new copy.
elt_H * add_elt_H(elt_H * h1, elt_H * h2);

// Addition operator.  Adds to first parameter.  No allocation.
void add_elt_H_new(elt_H * h1, elt_H * h2);

// Inverts an element of h.  No allocation.
void inverse(elt_H * h); 
 
// Return inverse of parameter as new copy.  
elt_H * inverse_new(elt_H * h);

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


/*
- Elements of G = H x< Sym_U
    - Implement as a tuple of elements of H and permutations.  Their could be more compact representations.
    - Op: Multiplication (should just be that of semi-direct products)
    - Op: Create S_i's.  Generate array of elements satisfying properties.
    - Op: Inverse (need to think about this)
- Elements of F[G]
    - Implement as a dictionary (hash table underlying?)
    - Op: Multiply (implement naively)
    - Op: Set element
    - Op: Get element
*/

#endif
