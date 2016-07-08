#ifndef __PUZZLE_H__
#define __PUZZLE_H__

#include "permutation.h"
#include <string.h>

typedef struct puzzle {
  perm * pi;
  int * puzzle;
  int row;
  int column;
}puzzle;

puzzle * create_puzzle_from_puzzle(puzzle * p, int row_index);

puzzle * create_puzzle(int rows, int cols);

puzzle * create_puzzle_from_file(const char * filename);

puzzle * create_puzzle_from_index(int row, int column, int index);

int get_column_from_row(int row_index, int col_index);

int print_puzzle(puzzle * p);

void write_puzzle(puzzle * p, int index);

void destroy_puzzle(puzzle * p);

int count_witnesses(puzzle * p);

#endif
