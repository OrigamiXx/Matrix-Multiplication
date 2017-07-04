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

int coor_to_index(int row1, int row2, int row3, int maxrow){
  return (row1-1)*pow(maxrow,2) + (row2-1)*maxrow + row3;
}
// 3dm_to_3cnf
int reduction_to_3cnf(int row, int column, int index, puzzle * p){
  FILE * cnf_file;
  cnf_file = fopen("%dby%dindex%d.cnf","w");
  assert(cnf_file != NULL);
  int i, j, k, m, l;
  int num_false_coor;
  num_false_coor = 0;
  // witness data from the puzzle from check_usp_rows
  for (i=1; i < row+1; i++){
    for (j=1; j < row+1; j++){
      for (k=1; k < row+1; k++){
        if (!check_usp_rows(i-1, j-1, k-1, p)){
          fprintf(cnf_file, "-%d -%d 0\n", coor_to_index(i, j, k, row), coor_to_index(i, j, k, row));
          num_false_coor++;
        }
      }
    }
  }
  // check uniqueness
  // each layer can only have one witness 3s*s(s-1)/2 combinations(comparisons)
  // x direction
  for (i=1; i < row+1; i++){
    for (j=1; j< row+1; j++){
      for (k=1; k< row; k++){
        for (l=j; l< row+1; l++){
          for (m=1; m < row+1; m++){
            if (coor_to_index(i,j,k,row) < coor_to_index(i,l,m,row)){
              fprintf(cnf_file, "-%d -%d 0\n", coor_to_index(i,j,k,row), coor_to_index(i,l,m,row));
            }
          }
        }
      }
    }
  }
  // y direction
  for (i=1; i < row+1; i++){
    for (j=1; j< row+1; j++){
      for (k=1; k< row; k++){
        for (l=j; l< row+1; l++){
          for (m=1; m < row+1; m++){
            if (coor_to_index(j,k,i,row) < coor_to_index(l,m,i,row)){
              fprintf(cnf_file, "-%d -%d 0\n", coor_to_index(j,k,i,row), coor_to_index(l,m,i,row));
            }
          }
        }
      }
    }
  }
  //z direction
  for (i=1; i < row+1; i++){
    for (j=1; j< row+1; j++){
      for (k=1; k< row; k++){
        for (l=j; l< row+1; l++){
          for (m=1; m < row+1; m++){
            if (coor_to_index(j,i,k,row) < coor_to_index(l,i,m,row)){
              fprintf(cnf_file, "-%d -%d 0\n", coor_to_index(j,i,k,row), coor_to_index(l,i,m,row));
            }
          }
        }
      }
    }
  }


  // existence
  // remain to complete  dont know how to turn a s-cnf to a 3cnf

}

int main(int argc, char * argv[]){
  int givenR = 8;
  int givenC = 6;
  int * puzzle1 = (int *) malloc(sizeof(int *)*givenR);
  puzzle1[0] = 279;
  puzzle1[1]= 284;
  puzzle1[2]= 290;
  puzzle1[3]= 297;
  puzzle1[4] = 318;
  puzzle1[5] = 350;
  puzzle1[6] = 389;
  puzzle1[7] = 487;
  //puzzle1[8] = 519;
  //puzzle1[9] = 586;
  //puzzle1[10] = 591;
  //puzzle1[11] = 630;
  //puzzle1[12] = 637;
  //puzzle1[13] = 642;
  puzzle * result = (puzzle *) (malloc(sizeof(puzzle)));
  result->row = givenR;
  result->column = givenC;
  result->pi = create_perm_identity(result->row);
  result -> puzzle = puzzle1;
  print_puzzle(result);

  destroy_puzzle(result);
  return 0;
}
