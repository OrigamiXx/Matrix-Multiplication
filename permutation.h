#ifndef __PERMUTATION_H__
#define __PERMUTATION_H__

typedef struct permutation {
  int size;
  int * arrow;
} permutation;

permutation * ID_permutation(int n);

int Apply_permutation(permutation * pi, int x);

permutation * next_permutation(permutation * pi);

// Replaces and returns the inverse of a permutation.
permutation * invert_permutation(permutation * pi);

permutation * compose(permutation * pi, permutation * delta);

int print(permutation * pi);

permutation * last_permutation(int n);

void destroy_perm(permutation * pi);

int equals(permutation * pi1, permutation * p2);

int is_last(permutation * pi);


#endif
