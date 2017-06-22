/*
  Tester for permutation module.

  Authors: Jerry, Matt.
*/


#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"

int main(){

  int verbose = false;

  printf("\n -- PERMUTATION TESTER --\n\n");
  
  printf("What size of permutation? ");
  int n;
  scanf("%d", &n);
  perm * f = create_perm_identity(n);
  perm * g = create_last_perm(n);

  printf("f is identity permutation:\n");
  print_perm_cycle(f);
  print_perm_tabular(f);
  printf("\ng is last permutation:\n");
  print_perm_cycle(g);
  print_perm_tabular(g);

  printf("\nWhat number (must be less than the size) do you want to apply in the permutation? ");
  int x;
  scanf("%d", &x);
  printf("\nThe output is: %d\n",apply_perm(f,x));
  printf("\nThe compose of g and f is:\n");
  print_perm_tabular(compose_perm(f,g));
  printf("\nNext permutation of f:\n");
  perm * f1 = next_perm(f);
  print_perm_tabular(f1);
  printf("\nNext of next permutation of f:\n");
  perm * f2 = next_perm(f1);
  print_perm_tabular(f2);

  int success = true;
  if (verbose) {
    printf("\nTest next and last\n");
  }
  
  perm * pi = create_perm_identity(n);
  int i = 1;
  if (verbose) {
    printf("Perm #%d\n",i);
    print_perm_tabular(pi);
  }
  while (!is_last_perm(pi)){
    pi = next_perm(pi);
    i++;
    if (verbose) {
      printf("Perm #%d\n",i);
      print_perm_cycle(pi);
      print_perm_tabular(pi);
    }

    perm * pi_inv = invert_perm(copy_perm(pi));
    perm * pi_tmp;
    pi_tmp = compose_perm(pi, pi_inv);
    int check1 = is_identity_perm(pi_tmp);
    destroy_perm(pi_tmp);
    pi_tmp = compose_perm(pi_inv, pi);
    int check2 = is_identity_perm(pi_tmp);
    destroy_perm(pi_tmp);
    pi_tmp = compose_perm(pi, pi);
    int check3 = is_identity_perm(pi) || is_identity_perm(pi_tmp) || !is_equals_perm(pi, pi_inv);
    destroy_perm(pi_tmp);
    destroy_perm(pi_inv);
    
    if (!check1 || !check2 || !check3) {
      printf("\nFAILURE -- compose, invert, is_identity, or is_equals is incorrect.\n");
      success = false;
    }
    
  }

  int fact = 1;
  for (int k = 1; k <= n; k++){
    fact *= k;
    if (fact < 1)
      printf("\nWARNING -- factorial computation overflow.\n");
  }

  if (i == fact)
    printf("\nSUCCESS -- Iterated over the correct number of permutations.\n");
  else
    printf("\nFAILURE -- Iterated incorrect number of permutations expected %d, found %d!\n", fact, i);  

  if (success)
    printf("\nSUCCESS -- compose, invert, is_identity, and is_equals are correct.\n");
  else
    printf("\nFAILURE -- compose, invert, is_identity, and is_equals is incorrect.\n");

  printf("\n -- PERMUTATION TESTS COMPLETE -- \n\n");
  return 0;
}
