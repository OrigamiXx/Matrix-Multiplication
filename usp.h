#ifndef __CHECKUSP_H__
#define __CHECKUSP_H__


#include "puzzle.h"
#include "set.h"
//typedef struct memo_table {
//  int*** M;
//  int same_perm;
//} memo_table;
int check_usp(puzzle * p);

int check_all_usp(int row, int column);

int check_usp_recursive(puzzle * p);

int find_witness(puzzle * p, int i1, set s2, set s3, int RR[20][20][20], int same_perm, int M[20][32][32]);
#endif



