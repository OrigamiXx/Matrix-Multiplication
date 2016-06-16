#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "CheckUSP.h"

int main(int argc, char * argv[]){
  
  //puzzle * p = create_puzzle(8,6); //6,3); // correct one (8,6)

  

  puzzle * p = create_puzzle_from_file("puzzles/usp_8_6.puz");
  print_puzzle(p);
  printf("result = %d\n",CheckUSP(p));

  return 0;

}
