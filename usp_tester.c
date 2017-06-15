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
  
   int * puzzle1 = (int *) malloc(sizeof(int *)*14);
  puzzle1[0] = 37;
  puzzle1[1]= 604;
  puzzle1[2]= 550;
  puzzle1[3]= 19;
  puzzle1[4] = 154;
  puzzle1[5] = 706;
  puzzle1[6] = 535;
  puzzle1[7] = 245;
  puzzle1[8] = 173;
  puzzle1[9] = 302;
  puzzle1[10] = 221;
  puzzle1[11] = 50;
  puzzle1[12] = 455;
  puzzle1[13] = 158;

  int givenR = 14;
  int givenC = 6;

  puzzle * result = (puzzle *) (malloc(sizeof(puzzle)));
  result->row = givenR;
  result->column = givenC;
  result->pi = create_perm_identity(result->row);
  result -> puzzle = puzzle1;

  print_puzzle(result);

  //check_usp(result);
  printf("%d", check_usp(result));
  
  return 0;

}
