#ifndef __CHECKUSP_H__
#define __CHECKUSP_H__


#include "puzzle.h"
#include "set.h"
#define MAX_ROW 8 
#define MAX_SET 1<<MAX_ROW
//typedef struct memo_table {
//  int*** M;
//  int same_perm;
//} memo_table;
int check_usp(puzzle * p);

int check_all_usp(int row, int column);

int check_usp_recursive(puzzle * p);

int find_witness(puzzle * p, int i1, set s2, set s3, int RR[20][20][20], int same_perm, int M[MAX_ROW][MAX_SET][MAX_SET][2]);

#endif



