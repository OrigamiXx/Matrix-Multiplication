#ifndef __3DM_TO_SAT_H__
#define __3DM_TO_SAT_H__

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
#include "puzzle.h"

int reduction_to_3cnf(FILE * file, int row, int column, int index, puzzle * p);

int reduction_simple(FILE * file, int row, int column, long index, puzzle * p);

bool popen_method(int row, int column, long index, puzzle * p);

bool popen_simple(int row, int column, long index, puzzle * p);

int file_making_methond(int row, int column, long index, puzzle * p);

int file_simple(int row, int column, long index, puzzle * p);

long power(int base, int exponent);

// direct interface with the solver
// true if is UNSAT which is a USP; false OW;
bool solver_simple(int row, int column, long index, puzzle * p);
#endif
