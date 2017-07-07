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


long power(int base, int exponent){
  long result = 1;
  for (int i = 0; i < exponent; i++){
    result = result * base;
  }
  return result;
}

int coor_to_index(int row1, int row2, int row3, int maxrow){
  return (row1-1)*power(maxrow,2) + (row2-1)*maxrow + (row3-1) + 1;
}


// 3dm_to_3cnf  O(s^5) clauses
int reduction_to_3cnf(FILE * file, int row, int column, int index, puzzle * p){
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
  //printf("%d\n", num_false_coor);
  clauses = num_false_coor + 1 + 3*row*(row*row-1)*(row*row)/2 + 3*row;
  fprintf(file, "p cnf %d %d\n", row*row*row, clauses);
  //printf("row to the third %d \n", row*row*row);
  //printf("row num %d\n", row);
  //printf("num of clauses %d\n", clauses);

  // # clauses: num_false_coor
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k <= row; k++){
        if (check_usp_rows(i-1, j-1, k-1, p)){ // check_usp_rows returning false indicates an edge is present
          fprintf(file, "-%d 0\n", coor_to_index(i, j, k, row));  // MWA: No need for extra literal.
        }
      }
    }
  }

  // check uniqueness
  // each layer can only have one witness 3s*s(s-1)/2 combinations(comparisons)
  // x direction
  // # clauses: s * (s^2 choose 2) = (s^2) * (s^2 - 1) / 2
  //int b = 0;
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k <= row; k++){
        for (l=1; l <= row; l++){
          for (m=1; m <= row; m++){
            if (coor_to_index(i,j,k,row) < coor_to_index(i,l,m,row)){
              fprintf(file, "-%d -%d 0\n", coor_to_index(i,j,k,row), coor_to_index(i,l,m,row));
              //b++;
            }
          }
        }
      }
    }
  }
  //printf("uniqueness clauses %d\n", b);
  //fprintf(cnf_file, "finish x direction\n");
  // y direction
  // # clauses: s * (s^2 choose 2) = (s^2) * (s^2 - 1) / 2
  //int a = 0;
  for (i=1; i <= row; i++){
    for (j=1; j<= row; j++){
      for (k=1; k<= row; k++){
        for (l=1; l<= row; l++){  // MWA: I think optimization with l wasn't correct.
          for (m=1; m <= row; m++){
            if (coor_to_index(j,k,i,row) < coor_to_index(l,m,i,row)){
              fprintf(file, "-%d -%d 0\n", coor_to_index(j,k,i,row), coor_to_index(l,m,i,row));
              //a++;
            }
          }
        }
      }
    }
  }
  //printf("uniqueness clauses %d\n", a);
  //fprintf(cnf_file, "finish y direction\n");
  //z direction
  // # clauses: s * (s^2 choose 2) = (s^2) * (s^2 - 1) / 2
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k <= row; k++){
        for (l=1; l <= row; l++){
          for (m=1; m <= row; m++){
            if (coor_to_index(j,i,k,row) < coor_to_index(l,i,m,row)){
              fprintf(file, "-%d -%d 0\n", coor_to_index(j,i,k,row), coor_to_index(l,i,m,row));
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
    fprintf(file,"-%d ",coor_to_index(i,i,i,row));
  }
  fprintf(file,"0\n");
  //fprintf(cnf_file, "finish diagonal direction\n");
  // existence
  // remain to complete  dont know how to turn a s-cnf to a 3cnf

  // # clauses: 3 * s
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k<= row; k++){
        fprintf(file, "%d ", coor_to_index(i,j,k,row));
      }
    }
    fprintf(file, "0\n");
  }
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k<= row; k++){
        fprintf(file, "%d ", coor_to_index(j,k,i,row));
      }
    }
    fprintf(file, "0\n");
  }
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      for (k=1; k<= row; k++){
        fprintf(file, "%d ", coor_to_index(j,i,k,row));
      }
    }
    fprintf(file, "0\n");
  }
}

// reduction from 3dm to 3cnf but with only O(s^3) clauses
// reduce the demension by using x y coordinates
int coor_converter(int type, int index1, int index2, int maxrow){
  if (type == 1){
    return (index1-1)*maxrow + (index2-1) + 1;
  }else if(type == 2){
    return maxrow*maxrow + (index1-1)*maxrow + (index2-1) + 1;
  }else{
    return 0;
  }
}


int reduction_simple(FILE * file, int row, int column, long index, puzzle * p){
  int i, j, k;
  int num_false_coor=0;
  int x = 1;
  int y = 2;
  for (i = 1; i<=row; i++){
    for (j = 1; j<=row; j++){
      for (k = 1; k<=row; k++){
        if (check_usp_rows(i-1,j-1,k-1,p)){
          num_false_coor++;
        }
      }
    }
  }

  int clauses;
  clauses = num_false_coor + 4*row*row*(row-1)/2.0 + 2*row + 1;
  fprintf(file, "p cnf %d %d\n", 2*row*row, clauses);

  //constraint
  for (i = 1; i<=row; i++){
    for (j = 1; j<=row; j++){
      for (k = 1; k<=row; k++){
        if (check_usp_rows(i-1,j-1,k-1,p)){
          fprintf(file, "-%d -%d 0\n", coor_converter(x, i, j, row), coor_converter(y, i, k, row));
        }
      }
    }
  }

  //uniqueness
  for (i=1; i<=row; i++){
    for (j=1; j<=row; j++){
      for (k=1; k<=row; k++){
        if(coor_converter(x, i, j, row) < coor_converter(x, i, k, row)){
          fprintf(file, "-%d -%d 0\n",coor_converter(x, i, j, row),coor_converter(x, i, k, row));
        }
      }
    }
  }
  for (i=1; i<=row; i++){
    for (j=1; j<=row; j++){
      for (k=1; k<=row; k++){
        if(coor_converter(x, j, i, row) < coor_converter(x, k, i, row)){
          fprintf(file, "-%d -%d 0\n",coor_converter(x, j, i, row),coor_converter(x, k, i, row));
        }
      }
    }
  }

  for (i=1; i<=row; i++){
    for (j=1; j<=row; j++){
      for (k=1; k<=row; k++){
        if(coor_converter(y, i, j, row) < coor_converter(y, i, k, row)){
          fprintf(file, "-%d -%d 0\n",coor_converter(y, i, j, row),coor_converter(y, i, k, row));
        }
      }
    }
  }
  for (i=1; i<=row; i++){
    for (j=1; j<=row; j++){
      for (k=1; k<=row; k++){
        if(coor_converter(y, j, i, row) < coor_converter(y, k, i, row)){
          fprintf(file, "-%d -%d 0\n",coor_converter(y, j, i, row),coor_converter(y, k, i, row));
        }
      }
    }
  }
  //existence
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      fprintf(file, "%d ", coor_converter(x, i, j, row));
    }
    fprintf(file, "0\n");
  }

  for (i=1; i <= row; i++){
    for (k=1; k <= row; k++){
      fprintf(file, "%d ", coor_converter(y, i, k, row));
    }
    fprintf(file, "0\n");
  }
  //diagonal 1c
  for (i = 1; i<= row; i++){
    fprintf(file, "-%d -%d ", coor_converter(x, i, i, row), coor_converter(y, i, i, row));
  }
  fprintf(file, "0\n");
}

bool popen_method(int row, int column, long index, puzzle * p){
  FILE * reduction = popen("minisat_solver > /dev/null 2>&1","w");

  if (reduction == NULL) {
    printf("Error, unable to executate minisat_solver\n");
  }
  reduction_to_3cnf(reduction, row, column, index, p);
  int result = pclose(reduction);
  if (result == 5120){

    return true;
  }else if (result== 2560){
    return false;
  }else{
    printf("wrong inputs!!!!!!\n");
    printf("%d\n",result);
    return false;
  }
  //return false;
}
bool popen_simple(int row, int column, long index, puzzle * p){
  FILE * reduction = popen("minisat_solver > /dev/null 2>&1","w");
  if (reduction == NULL) {
    printf("Error, unable to executate minisat_solver\n");
  }
  reduction_simple(reduction, row, column, index, p);
  int result = pclose(reduction);
  if (result == 5120){

    return true;
  }else if (result== 2560){
    return false;
  }else{
    printf("wrong inputs!!!!!!\n");
    printf("%d\n",result);
    return false;
  }
  //return false;
}
int file_making_methond(int row, int column, long index, puzzle * p){
  FILE * cnf_file;
  char *name;
  asprintf(&name, "%dby%dindex%ld.cnf",row,column,index);
  cnf_file = fopen(name,"w+");
  free(name);
  assert(cnf_file != NULL);
  reduction_to_3cnf(cnf_file, row, column, index, p);
  fclose(cnf_file);
  return 0;
}

int file_simple(int row, int column, long index, puzzle * p){
  FILE * cnf_file;
  char *name;
  asprintf(&name, "%dby%dindex%ld.cnf",row,column,index);
  cnf_file = fopen(name,"w+");
  free(name);
  assert(cnf_file != NULL);
  reduction_simple(cnf_file, row, column, index, p);
  fclose(cnf_file);
  return 0;
}


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
