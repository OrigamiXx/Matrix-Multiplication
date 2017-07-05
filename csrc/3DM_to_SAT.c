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



int coor_to_index(int row1, int row2, int row3, int maxrow){
  return (row1-1)*pow(maxrow,2) + (row2-1)*maxrow + (row3-1) + 1;
}
// 3dm_to_3cnf
int reduction_to_3cnf(int row, int column, int index, puzzle * p){
  FILE * cnf_file;
  char *name;
  asprintf(&name, "%dby%dindex%d.cnf",row,column,index);
  cnf_file = fopen(name,"w+");
  free(name);
  assert(cnf_file != NULL);
  int i, j, k, m, l;
  int num_false_coor = 0;
  int clauses;
  // witness data from the puzzle from check_usp_rows
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k <= row; k++){
        if (check_usp_rows(i-1, j-1, k-1, p)){ // check_usp_rows returning false indicates an edge is present
          num_false_coor++;
        }
      }
    }
  }
  printf("%d\n", num_false_coor);
  clauses = num_false_coor + 1 + 3*row*(row*row-1)*(row*row)/2 + 3*row; 
  fprintf(cnf_file, "p cnf %d %d\n", row*row*row, clauses);
  printf("row to the third %d \n", row*row*row);
  printf("row num %d\n", row);
  printf("num of clauses %d\n", clauses);
  
  // # clauses: num_false_coor
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k <= row; k++){
        if (check_usp_rows(i-1, j-1, k-1, p)){ // check_usp_rows returning false indicates an edge is present
          fprintf(cnf_file, "-%d 0\n", coor_to_index(i, j, k, row));  // MWA: No need for extra literal.
        }
      }
    }
  }

  // check uniqueness
  // each layer can only have one witness 3s*s(s-1)/2 combinations(comparisons)
  // x direction
  // # clauses: s * (s^2 choose 2) = (s^2) * (s^2 - 1) / 2
  int b = 0;
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){  
      for (k=1; k <= row; k++){
        for (l=1; l <= row; l++){
          for (m=1; m <= row; m++){
            if (coor_to_index(i,j,k,row) < coor_to_index(i,l,m,row)){
              fprintf(cnf_file, "-%d -%d 0\n", coor_to_index(i,j,k,row), coor_to_index(i,l,m,row));
              b++;
            }
          }
        }
      }
    }
  }
  printf("uniqueness clauses %d\n", b);
  //fprintf(cnf_file, "finish x direction\n");
  // y direction
  // # clauses: s * (s^2 choose 2) = (s^2) * (s^2 - 1) / 2
  int a = 0;
  for (i=1; i <= row; i++){
    for (j=1; j<= row; j++){
      for (k=1; k<= row; k++){
        for (l=1; l<= row; l++){  // MWA: I think optimization with l wasn't correct.
          for (m=1; m <= row; m++){
            if (coor_to_index(j,k,i,row) < coor_to_index(l,m,i,row)){
              fprintf(cnf_file, "-%d -%d 0\n", coor_to_index(j,k,i,row), coor_to_index(l,m,i,row));
              a++;
            }
          }
        }
      }
    }
  }
  printf("uniqueness clauses %d\n", a);
  //fprintf(cnf_file, "finish y direction\n");
  //z direction
  // # clauses: s * (s^2 choose 2) = (s^2) * (s^2 - 1) / 2
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k <= row; k++){
        for (l=1; l <= row; l++){
          for (m=1; m <= row; m++){
            if (coor_to_index(j,i,k,row) < coor_to_index(l,i,m,row)){
              fprintf(cnf_file, "-%d -%d 0\n", coor_to_index(j,i,k,row), coor_to_index(l,i,m,row));
            }
          }
        }
      }
    }
  }
  //fprintf(cnf_file, "finish z direction\n");
  // MWA: I think you also need to exclude the diagonal (1,1,1),
  // (2,2,2), ... (row,row,row) from being a witness, because it
  // corresponds to setting pi1 = pi2 = pi3.
  
  // MWA2: This wasn't quite right.  It is not that no entry on the
  // diagonal can be used.  It's that they can't all be used.  So the
  // contraint !\and_{i=1}^n x_{iii} must be true.
  
  // # clauses: 1
  for (i=1;i<=row;i++){
    fprintf(cnf_file,"-%d ",coor_to_index(i,i,i,row));
  }
  fprintf(cnf_file,"0\n");
  //fprintf(cnf_file, "finish diagonal direction\n");
  // existence
  // remain to complete  dont know how to turn a s-cnf to a 3cnf

  // # clauses: 3 * s
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k<= row; k++){
        fprintf(cnf_file, "%d ", coor_to_index(i,j,k,row));
      }
    }
    fprintf(cnf_file, "0\n");
  }
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k<= row; k++){
        fprintf(cnf_file, "%d ", coor_to_index(j,k,i,row));
      }
    }
    fprintf(cnf_file, "0\n");
  }
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k<= row; k++){
        fprintf(cnf_file, "%d ", coor_to_index(j,i,k,row));
      }
    }
    fprintf(cnf_file, "0\n");
  }





  fclose(cnf_file);
}


int main(int argc, char * argv[]){
  int givenR = 14;
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
  puzzle1[8] = 519;
  puzzle1[9] = 586;
  puzzle1[10] = 591;
  puzzle1[11] = 630;
  puzzle1[12] = 637;
  puzzle1[13] = 642;
  puzzle * result = (puzzle *) (malloc(sizeof(puzzle)));
  result->row = givenR;
  result->column = givenC;
  result->pi = create_perm_identity(result->row);
  result -> puzzle = puzzle1;
  print_puzzle(result);
  for (int r = 1; r <= givenR; r++)
    reduction_to_3cnf(r, givenC, get_indext_from_puzzle(result), result);
  /*if (check(result->puzzle, result->row, result->column)){
    printf("Yes\n");
  } else{
    printf("%s\n","No" );
  }*/


  destroy_puzzle(result);
  return 0;
}
