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

  elt_H * h1 = create_elt_H_random(4,3,3);
  
  print_elt_H(h1);

  elt_H *h2 = inverse_new(h1);
  
  print_elt_H(h2);

  elt_H * h3 = add_elt_H_new(h1,h2);

  print_elt_H(h3);

}
