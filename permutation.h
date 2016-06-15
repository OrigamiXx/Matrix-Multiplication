#ifndef __PERMUTATION_H__
#define __PERMUTATION_H__

typedef struct permutation {  // Rename: perm
  int size;
  int * arrow;
} permutation;

permutation * ID_permutation(int n);  // Rename: create_perm_identity

// Returns a newly allocated copy of pi.
permutation * copy_permutation(permutation * pi); // Rename: copy_perm

int Apply_permutation(permutation * pi, int x); // Rename: apply_perm

// Returns true iff pi is the identity permutation. // Rename: is_identity_perm
int is_identity_permutation(permutation * pi);

permutation * next_permutation(permutation * pi);  // Rename: next_perm



// Replaces and returns the inverse of a permutation.
permutation * inverse_permutation(permutation * pi);  // Rename: inverse_perm

permutation * compose(permutation * pi, permutation * delta); // Rename: compose_perm

int print(permutation * pi);  // Rename: print_perm

permutation * last_permutation(int n); // Rename: create_perm_last

void destroy_perm(permutation * pi);  // Ok

int equals(permutation * pi1, permutation * pi2); // Rename: is_equal_perm

int is_last_perm(permutation * pi); 


#endif
