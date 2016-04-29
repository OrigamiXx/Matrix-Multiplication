//#ifdef __PERMUTATION_H__
//#define __PERMUTATION_H__

typedef struct permutation {
  int size;
  int arrow[];
} permutation;

permutation * ID_permutation(int n);

int Apply_permutation(permutation * pi, int x);

//next_perm()

permutation * compose(permutation * pi, permutation * delta);

int print(permutation * pi);


//#endif
