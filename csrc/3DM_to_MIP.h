#ifndef __3DM_to_MIP_H__
#define __3DM_to_MIP_H__

/*
  This is a program that takes in a puzzle. The program would firstly convert
  the puzzle into a 3D matching problem then the 3D matching problem is solved
  through integer programming.

  Author: Anthony Yang Xu & Matt.
  Summer 2017.
*/

#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include "puzzle.h"

// Checks whether p is a strong USP, using a MIP reduction.  Returns
// IS_USP if p is a strong USP, NOT_USP if p is not a strong USP and
// UNDET_USP if it was unable to determine whether p is a strong USP.
// UNDET_USP will only be returned if there was an error.
check_t check_MIP(puzzle *p);

// A synchronized pthread version of check_MIP that takes it's
// parameters using an argument struct.  May be asynchronously
// interrupted by setting args -> interrupt = true.  Returns value
// computed via pthread_exit.  Return value only meaningful if not
// interrupted.
void * MIP(void * arguments);

// Deallocates the global Gurobi environment if allocated.
void finalize_check_MIP();

#endif
