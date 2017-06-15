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
  puzzle1[0] = 279;
  puzzle1[1]= 284;
  puzzle1[2]= 290;
  puzzle1[3]= 297;
  puzzle1[4] = 318;
  puzzle1[5] = 350;
  puzzle1[6] = 389;
  puzzle1[7] = 487;
  puzzle1[8] = 519;
  puzzle1[9] = 586;
  puzzle1[10] = 591;
  puzzle1[11] = 630;
  puzzle1[12] = 637;
  puzzle1[13] = 642;

  int givenR = 14;
  int givenC = 6;

  puzzle * result = (puzzle *) (malloc(sizeof(puzzle)));
  result->row = givenR;
  result->column = givenC;
  result->pi = create_perm_identity(result->row);
  result -> puzzle = puzzle1;

  print_puzzle(result);

  //topcheck_usp(result);
  printf("%d", check_usp(result));
  
  return 0;

}
