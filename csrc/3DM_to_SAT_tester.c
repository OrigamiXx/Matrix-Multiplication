/* Reduction from 3D-matching to 3SAT
   The goal is to use the result from check_usp_rows to deside whether
   witness is allowed to occur on that specific coordinate on a 3D cube.
   Then use the result to reduce this 3D-perfect matching problem to a 3SAT
   problem and print out the reduction in 3cnf-form (dimacs) for MapleSAT solver
   to give a final answer.

   Author: Jerry
*/
#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"
#include "usp.h"
#include "assert.h"
#include "puzzle.h"
#include <math.h>
#include "usp_bi.h"
#include "3DM_to_SAT.h"


int main(int argc, char * argv[]){
  int givenR = 3;
  int givenC = 3;
  // int * puzzle1 = (int *) malloc(sizeof(int *)*givenR);
  // puzzle1[0] = 279;
  // puzzle1[1]= 284;
  // puzzle1[2]= 290;
  // puzzle1[3]= 297;
  // puzzle1[4] = 318;
  // puzzle1[5] = 350;
  // puzzle1[6] = 389;
  // puzzle1[7] = 487;
  // puzzle1[8] = 519;
  // puzzle1[9] = 586;
  // puzzle1[10] = 591;
  // puzzle1[11] = 630;
  // puzzle1[12] = 637;
  // puzzle1[13] = 642;
  // puzzle * result = (puzzle *) (malloc(sizeof(puzzle)));
  // result->row = givenR;
  // result->column = givenC;
  // result->pi = create_perm_identity(result->row);
  // result -> puzzle = puzzle1;
  // print_puzzle(result);
  // file_simple(givenR,givenC, get_index_from_puzzle(result),result);

  int i, j;
  long index;
  i = givenC;
  j = givenR;
  //for (i = 1; i<=givenC; i++){
  //  for (int r = 1; r <= givenR; j++){
  long checked = 0;
  for (index; index < power(3, i*j) -1; index+=10){
    puzzle *p;
    p = create_puzzle_from_index(j,i,index);
    if (popen_simple(p->row, p->column,index,p) != popen_method(p->row, p->column,index,p)
        || check_usp_bi(p->puzzle, p->row, p->column)!= popen_simple(p->row, p->column,index,p)){
      printf("doesn't match for this case.\n");
      print_puzzle(p);
      break;
    }
    checked++;
    destroy_puzzle(p);
  }
  printf("%ld\n",checked);
  printf("finish checking%d by %d\n", j, i);
  //  }
  //}

  // if(popen_simple(givenR, givenC, get_index_from_puzzle(result),result)){
  //     printf("UNSAT\n");
  //   }else{
  //     printf("SAT\n");
  // }



  //reduction_to_3cnf(givenR, givenC, get_index_from_puzzle(result), result);
  /*if (check(result->puzzle, result->row, result->column)){
    printf("Yes\n");
  } else{
    printf("%s\n","No" );
  }*/


  //destroy_puzzle(result);
  return 0;
}
