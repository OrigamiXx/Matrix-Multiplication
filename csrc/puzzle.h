#ifndef __PUZZLE_H__
#define __PUZZLE_H__

#include "permutation.h"
#include <string.h>

//In this structure puzzle is stored in a 1D int array. In each cell, 
//there is an int that can be tansfer into a trinary index 
//of each row. E.g a 22 can be convert into 22=2*3^2+1*3^1+1*3^0=211
//=322 thus the raw is 322. If the row is longer than the converted 
//data, then all the digits that are not covered will be replaced 
//with 1. So for a 4 digit row a 22 would be 1322. 
typedef struct puzzle {
  perm * pi;
  int * puzzle;
  int row;
  int column;
}puzzle;

//create a puzzle that has one more row and same width as the input puzzle
// according to the given row_index
//Again, this row_index is the trinary index
puzzle * create_puzzle_from_puzzle(puzzle * p, int row_index);

//Creat an empty puzzle with given row and cols length
puzzle * create_puzzle(int rows, int cols);

//Read a file and return a puzzle. If the arguement is not a right file for
//puzzle, then do nothing
//Notice that the puzzle in the file is record as 1,2 or 3 since its for 
//readable reason. 
puzzle * create_puzzle_from_file(const char * filename);

//give width(column) and height(row) and the index of all the possible puzzle in this size
//return a puzzle
//index must be 0-(2^(k*s)-1)
//The index represents the whole puzzle
puzzle * create_puzzle_from_index(int row, int column, int index);

//Return the number at the col_index digit of the row with given row_index.
int get_column_from_row(int row_index, int col_index);

/*
 * Returns the result of setting the entry at c of row_index to val.
 */
int set_entry_in_row(int row_index, int c, int val);

int print_puzzle(puzzle * p);

//Write puzzle into a file
void write_puzzle(puzzle * p, int index);

//free puzzle
void destroy_puzzle(puzzle * p);

int count_witnesses(puzzle * p);

#endif