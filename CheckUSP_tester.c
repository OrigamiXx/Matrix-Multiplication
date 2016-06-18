#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "CheckUSP.h"
#include <math.h>
int main(int argc, char * argv[]){
  
  //puzzle * p = create_puzzle(8,6); //6,3); // correct one (8,6)

  

  puzzle * p = create_puzzle_from_file("puzzles/usp_8_6.puz");
  print_puzzle(p);
  //printf("result = %d\n",CheckUSP(p));
  // int row = 2;
  //int column = 3;
  //int index = 2;
  // puzzle * p = create_puzzle_from_index(row,column,index);
  //print_puzzle(p);
  //check_all_usp(row, column);
  return 0;

}
