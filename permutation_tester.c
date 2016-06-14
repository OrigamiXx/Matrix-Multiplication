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
  permutation * h = last_permutation(n);
  print(h);
  printf("What number(must be less than the size) do you want to apply in the permutation?\n");
  int x;
  scanf("%d", &x);
  printf("The output is:");
  printf("%d",Apply_permutation(f,x));
  printf("\n");
  printf("The compose of g o f is:\n");
  print(compose(f,g));
  printf("\n");
  printf("Next permutation of f:\n");
  permutation * f1 = next_permutation(f);
  print(f1);
  printf("Next permutation of f1:\n");
  permutation * f2 = next_permutation(f1);
  print(f2);

  printf("Test next and last\n");
  permutation * pi = ID_permutation(n);
  int i = 1;
  printf("Perm #%d\n",i);
  print(pi);
  for ( ; is_last(pi) ; ){
    pi = next_permutation(pi);
    i++;
    printf("Perm #%d\n",i);
    print(pi);
  }
  

  return 0;
}