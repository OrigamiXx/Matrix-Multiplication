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
#include "pthread.h"
#include "puzzle.h"
using namespace std;

typedef struct thread {
  puzzle * p;
  pthread_mutex_t complete_lock;
  pthread_mutex_t init_lock;
  pthread_mutex_t * cleanup_lock;
  
  void * solver_handle;
}thread;

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
int check_usp_bi(puzzle * p);

/*
 * Determines whether the given s-by-k puzzle U is a strong USP.
 * Checks using SAT and MIP solvers in parallel threads.
 */
int check_SAT_MIP(puzzle * p);

/*
 * Determines whether the given s-by-k puzzle U is a strong USP.
 * Tries to pick the most efficient method.  Uses cache if it has been
 * initialized -- call init_cache() to use this feature.  Uses the
 * bidirectional search if s is large enough, and the unidirectional
 * search otherwise.
 */
int check(puzzle_row U[], int s, int k);
int check(puzzle * p);

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
bool check2(puzzle_row r1, puzzle_row r2, int k);
bool check3(puzzle_row r1, puzzle_row r2, puzzle_row r3, int k);
bool check4(puzzle_row r1, puzzle_row r2, puzzle_row r3, puzzle_row r4, int k);

/*
 * A specialized function that determines whether any pair of rows
 * prevent an s-by-k from being a strong USP.  Return true iff all
 * pairs of rows are valid.
 */
bool check_row_pairs(puzzle_row U[], int s, int k);

/*
 * A specialized function that determines whether any triple of rows
 * prevent an s-by-k from being a strong USP.  Return true iff all
 * pairs of rows are valid.
 */
bool check_row_triples(puzzle_row U[], int s, int k);

/*
 * Returns true iff a length-k row that permutations map to u_1, u_2,
 * u_3, respectively, satisfies the inner condition of strong USPs.
 * It is false if this length-k row this mapping does not witness that
 * the puzzle is a strong USP.  Runtime is O(k).
 */
bool valid_combination(int u_1, int u_2, int u_3, int k);

/*
 * Heuristically precheck puzzle via random and greedy approaches
 * Returns 1 if puzzle is a strong USP.  Returns -1 if puzzle is not a
 * strong USP.  Returns 0 if the function has not determined the
 * puzzle is a strong USP.
 */
int random_precheck(bool * row_witness, int s, int k, int iter);


int count_row_witnesses(puzzle * p);

#endif
