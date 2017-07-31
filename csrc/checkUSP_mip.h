/*
  This is a program that takes in a puzzle. The program would firstly convert
  the puzzle into a 3D matching problem then the 3D matching problem is solved
  through integer programming.

  Anthony Yang Xy
  2017. 7. 13
*/

#include <stdio.h>
#include <stdlib.h>
#include "puzzle.h"

//Convert the 3D corrdiate into one index.
int corr_to_index(int s, int i, int j, int k);

//Return 0 if p is a nonUSP, 1 if p is a USP, -1 if error occurs.
int check_MIP(puzzle *p);

// Frees statically allocated data.
void finalize_check_MIP();

void * MIP(void * arguements);
void  mip_interrupt(void * solver_handle);
