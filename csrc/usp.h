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
 * Author: Matt & Jerry.
 */

#ifndef __USP_H__
#define __USP_H__

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
#include <semaphore.h>
#include "pthread.h"
#include "puzzle.h"
using namespace std;

typedef enum check_val {NOT_USP, IS_USP, UNDET_USP} check_t;

typedef struct thread {

  puzzle * p;
  bool interrupt;
  sem_t * complete_sem;
  bool complete;
  
} thread_args;


#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

#if GCC_VERSION > 40400
//Check number of USP's in puzzles from 1*column to max_row*column
int check_usp_same_col(int max_row, int column);
#endif


/*
 * Determines whether the given s-by-k puzzle U is a strong USP.  Uses
 * a unidirectional algorithm that tests all permutations pi_2 and
 * pi_3 to see whether they witness that U is not a strong USP.
 *
 * This is an alternative implementation of check_usp from usp.c.  It
 * uses c++ iterators with the built-in function next_permutation to
 * loop over all permutations.
 */
check_t check_usp_uni(puzzle * p);

/*
 * Determines whether the given s-by-k puzzle U is a strong USP.  Uses
 * a bidirectional algorithm.
 */
check_t check_usp_bi(puzzle * p);

/*
 * Determines whether the given s-by-k puzzle U is a strong USP.
 * Checks using SAT and MIP solvers in parallel threads.
 */
check_t check_SAT_MIP(puzzle * p);

/*
 * Determines whether the given s-by-k puzzle U is a strong USP.
 * Tries to pick the most efficient method.  Uses cache if it has been
 * initialized -- call init_cache() to use this feature.  Uses the
 * bidirectional search if s is large enough, and the unidirectional
 * search otherwise.
 */
check_t check(puzzle * p);

/*
 * Initialize the USP cache.  Precomputes whether each s-by-k puzzle
 * is a strong USP and stores the result in the cache.  Requires s <=
 * 4.
 */
void init_cache(int s, int k);

/*
 * Several specialized functions that determine whether a puzzle U is
 * a strong USP for a given constant number of rows.
 */
check_t check2(puzzle_row r1, puzzle_row r2, int k);
check_t check3(puzzle_row r1, puzzle_row r2, puzzle_row r3, int k);
check_t check4(puzzle_row r1, puzzle_row r2, puzzle_row r3, puzzle_row r4, int k);

/*
 * A specialized function that determines whether any pair of rows
 * prevent an s-by-k from being a strong USP.  Return true iff all
 * pairs of rows are valid.
 */
check_t check_row_pairs(puzzle * p);

/*
 * A specialized function that determines whether any triple of rows
 * prevent an s-by-k from being a strong USP.  Return true iff all
 * pairs of rows are valid.
 */
check_t check_row_triples(puzzle * p); 

/*
 * Returns true iff a length-k row that permutations map to u_1, u_2,
 * u_3, respectively, satisfies the inner condition of strong USPs.
 * It is false if this length-k row this mapping does not witness that
 * the puzzle is a strong USP.  Runtime is O(k).
 */
bool valid_combination(int u_1, int u_2, int u_3, int k);

/*
 * Heuristically precheck puzzle via random and greedy approaches
 * Returns IS_USP if puzzle is a strong USP.  Returns NOT_USP if
 * puzzle is not a strong USP.  Returns UNDET_USP if the function has
 * not determined the puzzle is a strong USP.
 */
check_t random_precheck(puzzle * p, int iter);

int count_row_witnesses(puzzle * p);

#endif
