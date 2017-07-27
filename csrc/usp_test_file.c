#include <stdio.h>
#include <stdlib.h>
#include "usp_bi.h"
#include "puzzle.h"

int main(int argc, char * argv[]){

  if (argc != 2){
    fprintf(stderr,"usage: usp_test_file <filename>\n");
    return -1;
  }
  
  puzzle * p = create_puzzle_from_file(argv[1]);
  if (p == NULL) {
    fprintf("Error: File does not exist or is not properly formated.\n");
    return -1;
  }
  
  print_puzzle(p);

  if (check(p->puzzle, p->row, p->column)){
    printf("is a strong USP.\n");
  } else {
    printf("is NOT a Strong USP.\n");
  }

  destroy_puzzle(p);
  
  return 0;

}
