#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include "permutation.h"
#include "groups.h"
#include <time.h>


int main(int argc, char * argv[]) {

  time_t t;
  srand((unsigned) time(&t));

  int U = 4;
  int k = 3;
  int m = 4;

  elt_H * h1 = create_elt_H_random(U,k,m);
  
  print_compact_elt_H(h1);

  elt_H *h2 = inverse_elt_H_new(h1);
  
  print_compact_elt_H(h2);

  elt_H * h3 = add_elt_H_new(h1,h2);

  print_compact_elt_H(h3);

  perm * p = create_perm_identity(U);

  elt_H * h4 = apply_elt_H_new(h1,p);

  print_compact_elt_H(h4);

  next_perm(p);


  elt_H * h5 = apply_elt_H_new(h1,p);
  print_perm_cycle(p);

  print_compact_elt_H(h5);

  p = invert_perm(p);

  elt_H * h6 = apply_elt_H_new(h5,p);
  print_perm_cycle(p);
 
  print_compact_elt_H(h6);

  inverse_elt_H(h6);

  add_elt_H(h6,h1);

  print_compact_elt_H(h6);

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

  printf("============= Finished Group H Tests ================\n");

  h1 = create_elt_H_random(U,k,m);
  p = create_perm_identity(U);
  next_perm(p);

  elt_G * g1 = create_elt_G(h1,p);
  
  print_compact_elt_G(g1);

  elt_G * g2 = copy_elt_G(g1);

  destroy_elt_G(g1);

  print_compact_elt_G(g2);

  elt_G * g3 = inverse_elt_G_new(g2);

  print_compact_elt_G(g3);

  elt_G * g4 = multiply_elt_G_new(g2,g3);

  print_compact_elt_G(g4);

  destroy_elt_G(g2);
  destroy_elt_G(g3);

  if (!is_identity_elt_G(g4)){
    printf("Test failed.\n");
  } else {
    printf("Test successful.\n");
  }

  destroy_elt_G(g4);

  printf("============= Finished Group G Tests ================\n");

  elt_KG * r1 = create_elt_KG_identity_zero(U,k,m);
 
  print_compact_elt_KG(r1);

  elt_KG * r2 = create_elt_KG_identity_one(U,k,m);
 
  print_compact_elt_KG(r2);

  elt_KG * r3 = copy_elt_KG(r2);

  destroy_elt_KG(r1);
  destroy_elt_KG(r2);

  
  print_compact_elt_KG(r3);

  elt_KG * r4 = add_elt_KG_new(r3,r3);

  destroy_elt_KG(r3);

  print_compact_elt_KG(r4);

  scalar_multiply_elt_KG(r4,7.0);

  print_compact_elt_KG(r4);

  elt_KG * r5 = multiply_elt_KG_new(r4,r4);

  destroy_elt_KG(r4);
  
  print_compact_elt_KG(r5);
 

  elt_G * g = create_elt_G(create_elt_H_random(U,k,m),create_last_perm(U));

  add_basis_elt_KG(r5,g,10);

  

  print_compact_elt_KG(r5);

  elt_KG * r6 = multiply_elt_KG_new(r5,r5);

  destroy_elt_KG(r5);

  print_compact_elt_KG(r6);

  elt_KG * r7 = multiply_elt_KG_new(r6,r6);
  printf("r7 = \n");
  print_compact_elt_KG(r7);

  elt_KG * r8 = multiply_elt_KG_new(r7,r7);
  printf("r8 = \n");
  print_compact_elt_KG(r8);

  elt_KG * r9 = multiply_elt_KG_new(r8,r8);
  printf("r9 = \n");
  print_compact_elt_KG(r9);

  elt_KG * r10 = multiply_elt_KG_new(r9,r9);
  printf("r10 = \n");
  print_compact_elt_KG(r10);

  
  elt_KG * r11 = multiply_elt_KG_new(r10,r10);
  printf("r11 = \n");
  print_compact_elt_KG(r11);


  destroy_elt_KG(r6);
  destroy_elt_KG(r7);
  destroy_elt_KG(r8);
  destroy_elt_KG(r9);
  destroy_elt_KG(r10);
  destroy_elt_KG(r11);
  
  printf("============= Finished Algebra K[G] Tests ================\n");

  return 0;
}
