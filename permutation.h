//#ifdef __PERMUTATION_H__
//#define __PERMUTATION_H__

typedef struct permutation {
  int size;
  int * arrow;
} permutation;

permutation * ID_permutation(int n);

int Apply_permutation(permutation * pi, int x);

permutation * next_permutation(permutation * pi);

permutation * compose(permutation * pi, permutation * delta);

int print(permutation * pi);

permutation * last_permutation(permutation *pi);


//#endif
