/*
 * Module implementing several non-trivial methods for checking
 * whether a given puzzle is a strong USP.
 *
 * 1. check_usp_uni() - Undirectional search for a witness that puzzle is not a strong USP.
 * 2. check_usp_bi() - Bidirectional search for a witness that puzzle is not a strong USP.
 * 3. check_SAT_MIP() - Thread parallel search using SAT and MIP solvers.
 * 4. check() - A combinations of the above checks and various heuristics.
 * 5. A mechanism for caching (that isn't turned on and may be broken).
 *
 * Checker should be named prefixed with "checker_". 
 *
 * Uses it's own representation of sets.
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

/*
 * Datatypes for expressing the return values of puzzle checkers and
 * heuristics.
 *
 * + IS_USP indicates that the puzzle is a strong USP.  
 * + NOT_USP indicates that the puzzle is not a strong USP.
 *
 * + UNDET_USP indicates that it was not determined whether or not the
 *     puzzle is a strong USP.  
 *
 *     Checkers should not return this value unless there is an
 *     intentional interruption of the function or an unrecoverable
 *     error.
 *
 *     Heuristics return this value in the event they are unable to
 *     decide whether the given puzzle is a strong USP.
 */
typedef enum check_val {NOT_USP, IS_USP, UNDET_USP} check_t;
typedef check_t (* checker_t)(puzzle *);


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

#ifdef __GUROBI_INSTALLED__
/*
 * Determines whether the given s-by-k puzzle U is a strong USP.
 * Checks using SAT and MIP solvers in parallel threads.
 * Requires GUROBI installed.
 */
check_t check_SAT_MIP(puzzle * p);
#endif

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
 * Returns true iff a length-k row that permutations map to u_1, u_2,
 * u_3, respectively, satisfies the inner condition of strong USPs.
 * It is false if this length-k row this mapping does not witness that
 * the puzzle is a strong USP.  Runtime is O(k).
 */
bool valid_combination(int u_1, int u_2, int u_3, int k);


/*
 *-------------------------------------------------------------
 *  Pair of Sets Implementation
 *-------------------------------------------------------------
 *
 * Below is an implementation of pairs of sets over universe of size
 * at most 31.  U <= {0, 1, 2, ... ,30}.  Each pair p = (S2, S3), for
 * S2, S3 <= U, is represented as a 64-bit long.  For a given p, i in
 * U is in S2 iff the ith bit (ordered lowest to hight) of p is 1.
 * For a given p, i in U is in S3 iff the (i+32)th bit of p is 1.
 * (Note: Bit index 31 stores a flag used by check_usp_bi to determine
 * whether the sets have been constructed in a way that corresponds to
 * selecting identical permutations.  Bit index 63 is unused.)
 */

typedef long set_long;

/*
 * All operations are implemented as compiler macros so that they are
 * as fast as possible. (Remark: Considering refactoring as c++ inline
 * functions instead of macros for robustness.)  All operations are
 * O(1) and accomplished without using loops.
 */

/*
 * Computes bitmask of an element a from {0, 1, ..., 30} into S2 or
 * S3.
 */
#define TO_S2(a) (1L << (a))
#define TO_S3(a) (1L << ((a) + 32))

/*
 * Returns true iff both sets are empty.
 */
#define SETS_EMPTY(a) ((a) == 0L)

/*
 * Returns a pair of full sets; S2 = S3 = {0, 1, 2, ..., 30}.
 */
#define CREATE_FULL(n) ((long)(CREATE_FULL_INT(n)) | (((long)CREATE_FULL_INT(n)) << 32))
#define CREATE_FULL_INT(n) ((n) == 32 ? ~0 : ~((1 << 31) >> (31 - (n))))

/*
 * Returns a pair of empty sets; S2 = S3 = {}.
 */
#define CREATE_EMPTY() (0L)

/*
 * Returns the pair that is the complement of the given pair. p = (S2,
 * S3) -> -p = (-S2, -S3).
 */
#define COMPLEMENT(s,n) (CREATE_FULL(n) ^ s)

/*
 * Accessors that check whether an element from {0, 1, ..., 30} is
 * part of S2 or S3.
 */
#define MEMBER_S2(s,a) (((s) & TO_S2(a)) != 0L)
#define MEMBER_S3(s,a) (((s) & TO_S3(a)) != 0L)

/*
 * Mutators that insert an element into S2 or S3, with no effect if
 * the element is already present.
 */
#define INSERT_S2(s,a) ((s) | TO_S2(a))
#define INSERT_S3(s,a) ((s) | TO_S3(a))

/*
 * Mutators that delete an element into S2 or S3, with no effect if
 * the element is not present.
 */
#define DELETE_S2(s,a) ((s) & ~(TO_S2(a)))
#define DELETE_S3(s,a) ((s) & ~(TO_S3(a)))

/*
 * Functions that twiddle the bit at index 31 and checks whether it is
 * set.
 */
#define SET_ID(s) (INSERT_S3(s,31))
#define UNSET_ID(s) (DELETE_S3(s,31))
#define IS_ID(s) (MEMBER_S3(s,31))


#endif
