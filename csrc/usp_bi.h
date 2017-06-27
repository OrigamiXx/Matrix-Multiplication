/* 
 * Module implementing several non-trivial methods for checking
 * whether a given puzzle is a strong USP.
 *
 * 1. Undirectional search for a witness that puzzle is not a strong USP.
 * 2. Bidirectional search for a witness that puzzle is not a strong USP.
 * 3. Hybrid search using 1 & 2 with the option to precompute and
 *    cache small puzzles.
 *
 * Uses it's own representation of sets and puzzles.
 *
 * Author: Matt.
 */

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

/*
 * Uses an integer to store a puzzle row, capable of representing rows
 * with at most 19 columns.
 */
typedef int puzzle_row;

/*
 * Prints an s-by-k puzzle U to the console.
 */
void printU(puzzle_row U[], int s, int k);

/*
 *
 * Determines whether the given s-by-k puzzle U is a strong USP.  Uses
 * a unidirectional algorithm that tests all permutations pi_2 and
 * pi_3 to see whether they witness that U is not a strong USP.
 *  
 * This is an alternative implementation of check_usp from usp.c.  It
 * uses c++ iterators with the built-in function next_permutation to
 * loop over all permutations.
 */
bool check_usp_uni(puzzle_row U[], int s, int k);

/* 
 * Determines whether the given s-by-k puzzle U is a strong USP.  Uses
 * a bidirectional algorithm.
 */
bool check_usp_bi(puzzle_row U[], int s, int k);

/* 
 * Determines whether the given s-by-k puzzle U is a strong USP.
 * Tries to pick the most efficient method.  Uses cache if it has been
 * initialized -- call init_cache() to use this feature.  Uses the
 * bidirectional search if s is large enough, and the unidirectional
 * search otherwise.
 */
bool check(puzzle_row U[], int s, int k);

/*
 * Initialize the USP cache.  Precomputes whether each s-by-k puzzle
 * is a strong USP and stores the result in the cache.  Requires s <=
 * 4.
 */
void init_cache(int k, int s);

/* 
 * Several specialized functions that determine whether a puzzle U is
 * a strong USP for a given constant number of rows.
 */
bool check2(puzzle_row r1, puzzle_row r2, int k);
bool check3(puzzle_row r1, puzzle_row r2, puzzle_row r3, int k);
bool check4(puzzle_row r1, puzzle_row r2, puzzle_row r3, puzzle_row r4, int k);





#endif