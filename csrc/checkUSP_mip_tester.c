#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c++.h"
#include "puzzle.h"
#include "usp.h"
#include "checkUSP_mip.h"
#include "permutation.h"


int main(int argc, char * argv[]){

  int givenR = 6;
  int givenC = 14;
  int * puzzle1 = (int *) malloc(sizeof(int *)*givenR);
  puzzle1[0] = 279;
  puzzle1[1]= 284;//284
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
  puzzle * result = (puzzle *) (malloc(sizeof(puzzle)));
  result->row = givenR;
  result->column = givenC;
  result -> puzzle = puzzle1;
  print_puzzle(result);

  DM_to_MIP(result);
  destroy_puzzle(result);

  }
