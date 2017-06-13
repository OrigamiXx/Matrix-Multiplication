#ifndef __CHECKUSP_H__
#define __CHECKUSP_H__


#include "puzzle.h"
#include "set.h"
#include <map>
#include <string>
#define MAX_ROW 8 
#define MAX_SET 1<<MAX_ROW
using namespace std;

//Check number of USP's in puzzles from 1*column to max_row*column
int check_usp_same_col(int max_row, int column);

//Check if given puzzle is a USP or not
int check_usp(puzzle * p);

//Check number of USP's on puzzles from 1*1 to row*column by using two methods
int check_all_usp(int row, int column);

int check_usp_recursive(puzzle * p);

bool find_witness(puzzle * p, int i1, set s2, set s3, int RR[20][20][20], bool same_perm, std::map<string, bool>&m);

string param_to_string(set s2, set s3, bool same_perm);

#endif



