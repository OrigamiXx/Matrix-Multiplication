#ifndef __PUZZLE_H__
#define __PUZZLE_H__

#include "permutation.h"
#include <string.h>

typedef struct puzzle {
  perm * pi;
  int ** puzzle;
  int row;
  int column;
}puzzle;

puzzle * create_puzzle(int rows, int cols);

puzzle * create_puzzle_from_file(char * filename);

puzzle * create_puzzle_from_index(int row, int column, int index);

int print_puzzle(puzzle * p);

void write_puzzle(puzzle * p, int index);

void destroy_puzzle(puzzle * p);


#endif
