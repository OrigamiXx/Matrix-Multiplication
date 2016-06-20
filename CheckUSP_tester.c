#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "CheckUSP.h"
#include <math.h>
#include "constants.h"
int main(int argc, char * argv[]){
  
  //puzzle * p = create_puzzle(8,6); //6,3); // correct one (8,6)

  

  //puzzle * p = create_puzzle_from_file("puzzles/test.puz");
  //print_puzzle(p);
  //if(CheckUSP(p)){
  //int i = CheckUSP(p);
  //printf("%d\n", i);

    //}

  
  //printf("result = %d\n",CheckUSP(p));
  //printf("%d\n",sizeof(int));
  //printf("%d\n",sizeof(long));
  //printf("%d\n",sizeof(long long));
  
  int row = 4;
  int column = 3;
  //int index = 1;
  //puzzle * p = create_puzzle_from_index(row,column,index);
  //print_puzzle(p);
  check_all_usp(row, column);
  return 0;

}
