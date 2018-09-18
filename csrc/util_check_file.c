#include <stdio.h>
#include <stdlib.h>
#include "checker.h"
#include "puzzle.h"
#include "3DM_to_SAT.h"
#include "3DM_to_MIP.h"
#include "heuristic.h"
#include "canonization.h"

int main(int argc, char * argv[]){

  if (argc != 2){
    fprintf(stderr,"usage: usp_test_file <filename>\n");
    return -1;
  }
  
  puzzle * p = create_puzzle_from_file(argv[1]);
  if (p == NULL) {
    fprintf(stderr,"Error: File does not exist or is not properly formated.\n");
    return -1;
  }
  
  print_puzzle(p);

  printf("-----\n");

  canonize_puzzle(p);

  print_puzzle(p);

  /*
  invalidate_tdm(p);
  compute_tdm(p);
  printf("Initial tdm:\n");
  print_tdm(p);
  printf("Simplified tdm:\n");
  simplify_tdm(p);
  print_tdm(p);
  printf("\n");
  */

  check_t res = check(p);
  if (res == IS_USP) {
    printf("is a strong USP.\n");
  } else if (res == NOT_USP) {
    printf("is NOT a Strong USP.\n");
  } else {
    printf("is UNDETERMINED.\n");
  }

  destroy_puzzle(p);
  
  return 0;

}
