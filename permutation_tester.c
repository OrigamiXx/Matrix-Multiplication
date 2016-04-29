#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"

int main(){
  printf("What's the size of the permutation?\n");
  int n;
  scanf("%d", &n);
  permutation * f = ID_permutation(n);
  permutation * g = ID_permutation(n);
  printf("print out the identity permutation:\n");
  printf("f is:\n");
  print(f);
  printf("g is:\n");
  print(g);
  printf("What number(must be less than the size) do you want to apply in the permutation?\n");
  int x;
  scanf("%d", &x);
  printf("The output is:");
  printf("%d",Apply_permutation(f,x));
  printf("\n");
  printf("The compose of g o f is:\n");
  print(compose(f,g));

  return 0;
}

