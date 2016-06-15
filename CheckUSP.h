#ifndef __CHECKUSP_H__
#define __CHECKUSP_H__


#include "permutation.h"

typedef struct puzzle {
  perm * pi;
  int ** puzzle;
  int row;
  int column;
}puzzle;

puzzle * create_puzzle(int rows, int cols);

puzzle * create_puzzle_from_file(char * filename);

#endif



