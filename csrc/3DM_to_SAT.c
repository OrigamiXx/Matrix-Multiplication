/* Reduction from 3D-matching to 3SAT
   The goal is to use the result from check_usp_rows to deside whether
   witness is allowed to occur on that specific coordinate on a 3D cube.
   Then use the result to reduce this 3D-perfect matching problem to a 3SAT
   problem and print out the reduction in 3cnf-form (dimacs) for MapleSAT solver
   to give a final answer.

   Author: Jerry & Matt.
*/
#include <stdio.h>
#include <stdlib.h>
#include "checker.h"
#include "assert.h"
#include "puzzle.h"
#include <pthread.h>
#include <semaphore.h>
#include "core/Dimacs.h"
#include "core/Solver.h"
#include "core/SolverTypes.h"
using namespace Minisat;

// Compute the variable number for x_{r1, r2}.
inline int var_num_x(int r1, int r2, int s){
  return r1 * s + r2;
}

// Compute the variable number for y_{r1, r3}.
inline int var_num_y(int r1, int r3, int s){
  return s * s + r1 * s + r3;
}

// Builds CNF formula in Solver S, which is satisfiable iff p -> tdm
// has a 3DM that is not the diagonal.  Uses the 2s^2 Boolean
// variables x_{r1,r2}, y_{r1,r3} for all r1, r2, r3 in [s].  Creates
// O(s^3) clauses.
void construct_reduction(puzzle * p, Solver * S){

  int s = p -> s;
  
  compute_tdm(p);
  
  // Allocate 2s^2 variables.
  for (int i = 0; i < 2 * s * s; i++)
    S -> newVar();
    
  vec<Lit> lits;
  lits.clear();

  // Constraints from three-dimensional matching.
  // x_{r1,r2} AND y_{r1,r3} <= tdm(r1,r2,r3) for all r1, r2, r3
  for (int r1 = 0; r1 < s; r1++)
    for (int r2 = 0; r2 < s; r2++) {
      int r2_var = var_num_x(r1, r2, s);
      for (int r3 = 0; r3 < s; r3++) {
	      int r3_var = var_num_y(r1, r3, s);
        if (!get_tdm_entry(p, r1, r2, r3)) {
          lits.clear();
          lits.push(~mkLit(r2_var));
          lits.push(~mkLit(r3_var));
          S -> addClause_(lits);
        }
      }
    }

  // Uniqueness of row / column indexes within layer r1.
  // -x_{r1,r21} OR -x_{r1,r22}  for all r1, r21 != r22
  // -y_{r1,r31} OR -y_{r1,r32}  for all r1, r31 != r32
  for (int r1 = 0; r1 < s; r1++)
    for (int a = 0; a < s; a++) {
      int r21_var = var_num_x(r1, a, s);
      int r31_var = var_num_y(r1, a, s);
      for (int b = 0; b < s; b++) {
        int r22_var = var_num_x(r1, b, s);
        int r32_var = var_num_y(r1, b, s);
        if (r21_var < r22_var){ // Implies r31_var < r32_var
          lits.clear();
          lits.push(~mkLit(r21_var));
          lits.push(~mkLit(r22_var));
          S -> addClause_(lits);
          lits.clear();
          lits.push(~mkLit(r31_var));
          lits.push(~mkLit(r32_var));
          S -> addClause_(lits);
        }
      }
    }

  // Uniqueness of row / column indexes within slice r.
  // -x_{r11,r2} OR -x_{r12,r2}  for all r2, r11 != r12
  // -y_{r11,r3} OR -y_{r12,r3}  for all r3, r11 != r12
  for (int r = 0; r < s; r++)
    for (int r11 = 0; r11 < s; r11++){
      int r21_var = var_num_x(r11, r, s);
      int r31_var = var_num_y(r11, r, s);
      for (int r12 = 0; r12 < s; r12++){
        int r22_var = var_num_x(r12, r, s);
        int r32_var = var_num_y(r12, r, s);
        if (r21_var < r22_var){ // Implies r31_var < r32_var
          lits.clear();
          lits.push(~mkLit(r21_var));
          lits.push(~mkLit(r22_var));
          S -> addClause_(lits);
          lits.clear();
          lits.push(~mkLit(r31_var));
          lits.push(~mkLit(r32_var));
          S -> addClause_(lits);
        }
      }
    }

  // Existence in layer r1.
  // OR_{r2} x_{r1,r2}  for all r1.
  // OR_{r3} y_{r1,r3}  for all r1.
  
  for (int r1 = 0; r1 < s; r1++){
    lits.clear();
    for (int r2 = 0; r2 < s; r2++)
      lits.push(mkLit(var_num_x(r1, r2, s)));
    S -> addClause_(lits);

    lits.clear();
    for (int r3 = 0; r3 < s; r3++)
      lits.push(mkLit(var_num_y(r1, r3, s)));
    S -> addClause_(lits);
  }

  // Existence in slice r.
  // OR_{r1} x_{r1, r2} for all r2.
  // OR_{r1} y_{r1, r3} for all r3.
  for (int r = 0; r < s; r++){
    lits.clear();
    for (int r1 = 0; r1 < s; r1++)
      lits.push(mkLit(var_num_x(r1, r, s)));
    S -> addClause_(lits);

    lits.clear();    
    for (int r1 = 0; r1 < s; r1++)
      lits.push(mkLit(var_num_y(r1, r, s)));
    S -> addClause_(lits);
  }
    
  // Not the diagonal.
  // OR_{r1} (-x_{r1,r1} or -y_{r1,r1})  for all r1.
  lits.clear();
  for (int r1 = 0; r1 < s; r1++){
    lits.push(~mkLit(var_num_x(r1, r1, s)));
    lits.push(~mkLit(var_num_y(r1, r1, s)));
  }
  S -> addClause_(lits);

}

// Checks whether p is a strong USP using the provided Solver.
// Returns IS_USP if p is a strong USP, NOT_USP if p is not a strong
// USP and UNDET_USP if it was unable to determine whether p is a
// strong USP.  UNDET_USP will only be returned if the solving was
// interrupted, or there was an error.
check_t check_SAT(puzzle * p, Solver * S){

  S -> verbosity = 0;

  construct_reduction(p, S);
  
  if (!(S -> simplify())){
    return IS_USP;
  } else {
    lbool ret = S -> solveLimited();
    if (ret == l_True)
      return NOT_USP;
    else if(ret == l_False)
      return IS_USP;
    else 
      return UNDET_USP;
    
  }
  
}

// Checks whether p is a strong USP, using a SAT solver.  Returns
// IS_USP if p is a strong USP, NOT_USP if p is not a strong USP and
// UNDET_USP if it was unable to determine whether p is a strong USP.
// UNDET_USP will only be returned if there was an error.
check_t check_SAT(puzzle * p){

  Solver * S = new Solver();
  check_t res = check_SAT(p, S);
  delete S;
  return res;

}

// A synchronized pthread version of check_SAT that takes it's
// parameters using an argument struct.  May be asynchronously
// interrupted by setting args -> interrupt = true.  Returns value
// computed via pthread_exit.  Return value only meaningful if not
// interrupted.
void *SAT(void *arguments){

  struct thread *args = (struct thread *)arguments;

  Solver * S = new Solver();
  S -> setInterruptPtr(&(args->interrupt));
  
  check_t res = check_SAT(args->p, S);

  delete S;

  args -> complete = true;
  sem_post(args -> complete_sem);

  pthread_exit((void*)res);
}
