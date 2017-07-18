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

int DM_to_MIP(puzzle *p);