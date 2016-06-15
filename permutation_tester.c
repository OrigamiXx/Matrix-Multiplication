#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"

int main(){
  printf("What's the size of the permutation?\n");
  int n;
  scanf("%d", &n);
  perm * f = create_perm_identity(n);
  perm * g = create_perm_identity(n);
  printf("print out the identity permutation:\n");
  printf("f is:\n");
  print_perm(f);
  printf("g is:\n");
  print_perm(g);
  perm * h = create_last_perm(n);
  print_perm(h);
  printf("What number(must be less than the size) do you want to apply in the permutation?\n");
  int x;
  scanf("%d", &x);
  printf("The output is:");
  printf("%d",apply_perm(f,x));
  printf("\n");
  printf("The compose of g o f is:\n");
  print_perm(compose_perm(f,g));
  printf("\n");
  printf("Next permutation of f:\n");
  perm * f1 = next_perm(f);
  print_perm(f1);
  printf("Next permutation of f1:\n");
  perm * f2 = next_perm(f1);
  print_perm(f2);

  printf("Test next and last\n");
  perm * pi = create_perm_identity(n);
  int i = 1;
  printf("Perm #%d\n",i);
  print_perm(pi);
  while (!is_last_perm(pi)){
    pi = next_perm(pi);
    i++;
    printf("Perm #%d\n",i);
    print_perm(pi);
  }
  

  return 0;
}
