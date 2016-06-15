#ifndef __PERMUTATION_H__
#define __PERMUTATION_H__

typedef struct perm {  // Rename: perm
  int size;
  int * arrow;
} perm;

perm * create_perm_identity(int n);  // Rename: create_perm_identity

// Returns a newly allocated copy of pi.
perm * copy_perm(perm * pi); // Rename: copy_perm

int apply_perm(perm * pi, int x); // Rename: apply_perm

// Returns true iff pi is the identity perm. // Rename: is_identity_perm
int is_identity_perm(perm * pi);

perm * next_perm(perm * pi);  // Rename: next_perm

// Replaces and returns the inverse of a perm.
perm * inverse_perm(perm * pi);  // Rename: inverse_perm

perm * compose_perm(perm * pi, perm * delta); // Rename: compose_perm

int print_perm(perm * pi);  // Rename: print_perm

perm * create_last_perm(int n); // Rename: create_perm_last

void destroy_perm(perm * pi);  // Ok

int is_equals_perm(perm * pi1, perm * pi2); // Rename: is_equal_perm

int is_last_perm(perm * pi); 

void print_compact_perm(permutation * pi);

#endif
