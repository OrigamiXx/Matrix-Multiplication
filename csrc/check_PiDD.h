#include "puzzle.h"
#include "checker.h"

/*
 * Returns IS_USP if p is a strong USP, and NOT_USP otherwise.  Uses
 * PiDDs to represent the sets of bad permutations for each column and
 * then verifies that the intersection of such sets is the singleton
 * set with the identity.  Not particular efficient for s >= 10.
 */
check_t check_PiDD(puzzle * p);

void search_PiDD(int s);
