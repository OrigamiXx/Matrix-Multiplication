#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "usp_construct.h"
#include "checker.h"
#include "3DM_to_MIP.h"
#include "3DM_to_SAT.h"

int main(int argc, char* argv[]){

  /*
  int s1 = 10;
  int k1 = 6;

  int s2 = 8;
  int k2 = 5;

  int s3 = 4;
  int k3 = 4;

  
  long seed = time(NULL);
  srand48(seed);
  
  puzzle * p1 = create_puzzle(s1,k1);
  puzzle * p2 = create_puzzle(s2,k2);
  puzzle * p3 = create_puzzle(s3,k3);

  int usp = 0;
  int not_usp = 0;
  
  for (int i = 0; i < 1000000; i++){
  
    random_usp(p1);
    //arrange_puzzle(p1);

    //randomize_puzzle(p1);
    random_usp(p2);
    //arrange_puzzle(p2);
    //randomize_puzzle(p2);

    //random_usp(p3);
    
    print_puzzle(p1);
    printf("\n");
    print_puzzle(p2);
    printf("\n");
    

    //puzzle * pa = create_puzzle_product(p1, p2);
    //puzzle * pb = create_puzzle_product(pa, p3);
    puzzle * p =  create_puzzle_product(p1, p2);
    //puzzle * p = create_puzzle_paired(p1);
    //print_puzzle(p);
    
    if (check(p) == IS_USP) {
      //printf("Is a USP!\n");
      usp++;
    } else {
      printf("Is not a USP.\n");
      print_puzzle(p1);
      printf("\n");
      print_puzzle(p2);
      printf("\n");
      print_puzzle(p3);
      printf("\n");
      not_usp++;
      assert(check(p1) == IS_USP && check(p2) == IS_USP && check(p3) == IS_USP);
    }
    //destroy_puzzle(pa);
    //destroy_puzzle(pb);
    destroy_puzzle(p);
    
    printf("usp: %d, not_usp: %d\n", usp, not_usp);
  }
  */
  
  puzzle * p1 = create_puzzle_from_file("puzzles/14-6-1.puz");
  //puzzle * p2 = create_puzzle_from_file("puzzles/14-6-2.puz");
  puzzle * p2 = create_puzzle(8,5);
  random_usp(p2);

  print_puzzle(p2);
  
  puzzle * p = create_puzzle_product(p1,p2);

  print_puzzle(p);
  
  if (check_MIP(p) == IS_USP) {
    printf("Product is a USP!\n");
  } else {
    printf("Product is not a USP.\n");
  }
  
  return 0;
}
