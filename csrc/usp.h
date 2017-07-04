#ifndef __CHECKUSP_H__
#define __CHECKUSP_H__


#include "puzzle.h"
#include "set.h"
#include <map>
#include <string>
#define MAX_ROW 8
#define MAX_SET 1<<MAX_ROW
using namespace std;

#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

#if GCC_VERSION > 40400
//Check number of USP's in puzzles from 1*column to max_row*column
int check_usp_same_col(int max_row, int column);
#endif

//check one case of pi1 pi2 pi3 holds the strong usp property
int check_usp_rows(int row1, int row2, int row3, puzzle * p);


//Check if given puzzle is a USP or not
//Return 1 if p is a USP, else return 0
int check_usp(puzzle * p);

//Check number of USP's on puzzles from 1*1 to row*column by using two methods
int check_all_usp(int row, int column, int returnP);

int check_usp_recursive(puzzle * p);

bool find_witness(puzzle * p, int i1, set s2, set s3, int RR[20][20][20], bool same_perm, std::map<string, bool>&m);

string param_to_string(set s2, set s3, bool same_perm);

#endif
