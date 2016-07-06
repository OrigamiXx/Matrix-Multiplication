#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include <map>
int main(int argc, char * argv[]){
  
  //puzzle * p = create_puzzle(8,6); //6,3); // correct one (8,6)

  

  //puzzle * p = create_puzzle_from_file("puzzles/test.puz");
  //print_puzzle(p);
  //if(check_usp_recursive(p)){
  //int i = check_usp(p);
  //printf("hello%d\n", i);
  //}

  
  //printf("result = %d\n",check_usp(p));
  //printf("%d\n",sizeof(int));
  //printf("%d\n",sizeof(long));
  //printf("%d\n",sizeof(long long));
  
  int row = 5;
  int column = 5;
  //int index = 1;
  //puzzle * p = create_puzzle_from_index(row,column,index);
  //print_puzzle(p);
  //check_all_usp(row, column);
  check_usp_same_col(row,column);
  
  return 0;

}
