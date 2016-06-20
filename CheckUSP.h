#ifndef __CHECKUSP_H__
#define __CHECKUSP_H__


#include "permutation.h"
#include <math.h>
typedef struct puzzle {
  perm * pi;
  int ** puzzle;
  int row;
  int column;
}puzzle;

puzzle * create_puzzle(int rows, int cols);

puzzle * create_puzzle_from_file(char * filename);

puzzle * create_puzzle_from_index(int row, int column, int index);

int CheckUSP(puzzle * p);

int print_puzzle(puzzle * p);

int check_all_usp(int row, int column);

void write_puzzle(puzzle * p, int index);

void destroy_puzzle(puzzle * p);

#endif



