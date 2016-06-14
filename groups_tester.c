#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include "permutation.h"
#include "groups.h"
#include <time.h>


void main(int argc, char * argv[]) {

  time_t t;
  srand((unsigned) time(&t));

  int U = 4;
  int k = 3;
  int m = 3;

  elt_H * h1 = create_elt_H_random(U,k,m);
  
  print_elt_H(h1);

  elt_H *h2 = inverse_elt_H_new(h1);
  
  print_elt_H(h2);

  elt_H * h3 = add_elt_H_new(h1,h2);

  print_elt_H(h3);

  permutation * p = ID_permutation(U);

  elt_H * h4 = apply_elt_H_new(h1,p);

  print_elt_H(h4);

  p = next_permutation(p);


  elt_H * h5 = apply_elt_H_new(h1,p);
  print(p);

  print_elt_H(h5);

  p = invert_permutation(p);

  elt_H * h6 = apply_elt_H_new(h5,p);
  print(p);
 
  print_elt_H(h6);

  inverse_elt_H(h6);

  add_elt_H(h6,h1);

  print_elt_H(h6);

  if (!is_identity_elt_H(h6)){
    printf("Test failed.\n");
  } else {
    printf("Test successful.\n");
  }


  destroy_elt_H(h1);
  destroy_elt_H(h2);
  destroy_elt_H(h3);
  destroy_elt_H(h4);
  destroy_elt_H(h5);
  destroy_elt_H(h6);
  destroy_perm(p);

}
