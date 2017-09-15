#ifndef __3DM_TO_SAT_H__
#define __3DM_TO_SAT_H__

/* Reduction from 3D-matching to 3SAT
   The goal is to use the result from check_usp_rows to deside whether
   witness is allowed to occur on that specific coordinate on a 3D cube.
   Then use the result to reduce this 3D-perfect matching problem to a 3SAT
   problem and print out the reduction in 3cnf-form (dimacs) for MapleSAT solver
   to give a final answer.

   Author: Jerry & Matt.
*/
#include <stdlib.h>
#include "puzzle.h"
#include "checker.h"

// Checks whether p is a strong USP, using a SAT solver.  Returns
// IS_USP if p is a strong USP, NOT_USP if p is not a strong USP and
// UNDET_USP if it was unable to determine whether p is a strong USP.
// UNDET_USP will only be returned if there was an error.
check_t check_SAT(puzzle * p);

// A synchronized pthread version of check_SAT that takes it's
// parameters using an argument struct.  May be asynchronously
// interrupted by setting args -> interrupt = true.  Returns value
// computed via pthread_exit.  Return value only meaningful if not
// interrupted.
void *SAT(void *arguments);

#endif
