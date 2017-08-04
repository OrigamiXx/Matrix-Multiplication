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
#include <pthread.h>
#include <syscall.h>

#include <errno.h>
#include <signal.h>
#include "utils/System.h"
#include "utils/ParseUtils.h"
#include "core/Dimacs.h"
#include "core/Solver.h"
#include "core/SolverTypes.h"
using namespace Minisat;

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
void reduction_to_3cnf(FILE * file, int row, int column, int index, puzzle * p){
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


void reduction_simple(FILE * file, int row, int column, long index, puzzle * p){
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

// direct interface with the solver
// true if is UNSAT which is a USP; false OW;



int check_SAT(puzzle * p, Solver * S){
  int row = p->row;
  //int column = p->column;
  int i, j, k;
  //int num_false_coor=0;
  int x = 1;
  int y = 2;

  S -> verbosity = 0;

  vec<Lit> lits;
  int var, lit;
  lits.clear();
  //constraint
  for (i = 1; i<=row; i++){
    for (j = 1; j<=row; j++){
      for (k = 1; k<=row; k++){
        if (check_usp_rows(i-1,j-1,k-1,p)){
          //fprintf(file, "-%d -%d 0\n", coor_converter(x, i, j, row), coor_converter(y, i, k, row));
          lits.clear();
          lit = -coor_converter(x, i, j, row);
          var = coor_converter(x, i, j, row)-1;
          while (var >= S -> nVars()) S -> newVar();
          lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
          lit = -coor_converter(y, i, k, row);
          var = coor_converter(y, i, k, row)-1;
          while (var >= S -> nVars()) S -> newVar();
          lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
          S -> addClause_(lits);
        }
      }
    }
  }

  //uniqueness
  for (i=1; i<=row; i++){
    for (j=1; j<=row; j++){
      for (k=1; k<=row; k++){
        if(coor_converter(x, i, j, row) < coor_converter(x, i, k, row)){
          //fprintf(file, "-%d -%d 0\n",coor_converter(x, i, j, row),coor_converter(x, i, k, row));
          lits.clear();
          lit = -coor_converter(x, i, j, row);
          var = coor_converter(x, i, j, row)-1;
          while (var >= S -> nVars()) S -> newVar();
          lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
          lit = -coor_converter(x, i, k, row);
          var = coor_converter(x, i, k, row)-1;
          while (var >= S -> nVars()) S -> newVar();
          lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
          S -> addClause_(lits);
        }
      }
    }
  }
  for (i=1; i<=row; i++){
    for (j=1; j<=row; j++){
      for (k=1; k<=row; k++){
        if(coor_converter(x, j, i, row) < coor_converter(x, k, i, row)){
          //fprintf(file, "-%d -%d 0\n",coor_converter(x, j, i, row),coor_converter(x, k, i, row));
          lits.clear();
          lit = -coor_converter(x, j, i, row);
          var = coor_converter(x, j, i, row)-1;
          while (var >= S -> nVars()) S -> newVar();
          lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
          lit = -coor_converter(x, k, i, row);
          var = coor_converter(x, k, i, row)-1;
          while (var >= S -> nVars()) S -> newVar();
          lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
          S -> addClause_(lits);
        }
      }
    }
  }

  for (i=1; i<=row; i++){
    for (j=1; j<=row; j++){
      for (k=1; k<=row; k++){
        if(coor_converter(y, i, j, row) < coor_converter(y, i, k, row)){
          //fprintf(file, "-%d -%d 0\n",coor_converter(y, i, j, row),coor_converter(y, i, k, row));
          lits.clear();
          lit = -coor_converter(y, i, j, row);
          var = coor_converter(y, i, j, row)-1;
          while (var >= S -> nVars()) S -> newVar();
          lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
          lit = -coor_converter(y, i, k, row);
          var = coor_converter(y, i, k, row)-1;
          while (var >= S -> nVars()) S -> newVar();
          lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
          S -> addClause_(lits);
        }
      }
    }
  }
  for (i=1; i<=row; i++){
    for (j=1; j<=row; j++){
      for (k=1; k<=row; k++){
        if(coor_converter(y, j, i, row) < coor_converter(y, k, i, row)){
          //fprintf(file, "-%d -%d 0\n",coor_converter(y, j, i, row),coor_converter(y, k, i, row));
          lits.clear();
          lit = -coor_converter(y, j, i, row);
          var = coor_converter(y, j, i, row)-1;
          while (var >= S -> nVars()) S -> newVar();
          lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
          lit = -coor_converter(y, k, i, row);
          var = coor_converter(y, k, i, row)-1;
          while (var >= S -> nVars()) S -> newVar();
          lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
          S -> addClause_(lits);
        }
      }
    }
  }
  //existence
  lits.clear();
  for (i=1; i <= row; i++){
    for (j=1; j <= row; j++){
      //fprintf(file, "%d ", coor_converter(x, i, j, row));
      lit = coor_converter(x, i, j, row);
      var = coor_converter(x, i, j, row)-1;
      while (var >= S -> nVars()) S -> newVar();
      lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
    }
    //fprintf(file, "0\n");
    S -> addClause_(lits);
    lits.clear();
  }
  lits.clear();
  for (i=1; i <= row; i++){
    for (k=1; k <= row; k++){
      //fprintf(file, "%d ", coor_converter(y, i, k, row));
      lit = coor_converter(y, i, k, row);
      var = coor_converter(y, i, k, row)-1;
      while (var >= S -> nVars()) S -> newVar();
      lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
    }
    //fprintf(file, "0\n");
    S -> addClause_(lits);
    lits.clear();
  }
  //diagonal 1c
  lits.clear();
  for (i = 1; i<= row; i++){
    //fprintf(file, "-%d -%d ", coor_converter(x, i, i, row), coor_converter(y, i, i, row));
    lit = -coor_converter(x, i, i, row);
    var = coor_converter(x, i, i, row)-1;
    while (var >= S -> nVars()) S -> newVar();
    lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
    lit = -coor_converter(y, i, i, row);
    var = coor_converter(y, i, i, row)-1;
    while (var >= S -> nVars()) S -> newVar();
    lits.push( (lit > 0) ? mkLit(var) : ~mkLit(var) );
  }
  //fprintf(file, "0\n");
  S -> addClause_(lits);
  lits.clear();

  if (!(S -> simplify())){
    return 1;
  }else{
    vec<Lit> dummy;
    /*lbool ret = S -> solveLimited(dummy);
      if (ret == l_True){
      return 0;
      }else if(ret == l_False){
      return 1;
      }*/
    lbool ret = S -> solveLimited();
    if (ret == l_True){
      return 0;
    } else if(ret == l_False){
      return 1;
    } else {
      return -1;
    }
  }
}

int check_SAT(puzzle * p){

  Solver * S = new Solver();
  int res = check_SAT(p, S);
  delete S;
  return res;

}

void *SAT(void *arguments){
  struct thread *args = (struct thread *)arguments;

  Solver * S = new Solver();
  args -> solver_handle = S;
  pthread_mutex_unlock(&(args->init_lock));

  //sleep(10000);
  int res = check_SAT(args->p, S);

  pthread_mutex_lock(args -> cleanup_lock);
  args -> solver_handle = NULL;
  delete S;

  pthread_mutex_unlock(&(args->complete_lock));

  pthread_exit((void*)res);
}

void sat_interrupt(void * solver_handle){

  if (solver_handle != NULL) {
    //printf("Interrupting\n");
    ((Solver *)solver_handle) -> interrupt();
    //printf("Done Interrupting\n");
  }

}
