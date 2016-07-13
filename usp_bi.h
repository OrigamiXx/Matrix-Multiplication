#ifndef __USP_BI_H__
#define __USP_BI_H__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <set>
#include <string>
#include <iostream>
#include <map>
#include <math.h>
#include <assert.h>

using namespace std;

typedef int puzzle_row;

bool check_usp_uni(puzzle_row U[], int s, int k);
bool check_usp_bi(puzzle_row U[], int s, int k);

bool check(puzzle_row U[], int s, int k);
bool check2(puzzle_row r1, puzzle_row r2, int k);
bool check3(puzzle_row r1, puzzle_row r2, puzzle_row r3, int k);
bool check4(puzzle_row r1, puzzle_row r2, puzzle_row r3, puzzle_row r4, int k);

void printU(puzzle_row U[], int s, int k);

void init_cache(int k, int s);


#endif
