/*
 * Module implementing several methods for combining USPs into larger
 * USPs.
 *
 * Author: Matt.
 */

#ifndef __SEARCH_GREEDY_H__
#define __SEARCH_GREEDY_H__

#include <stdio.h>
#include <stdlib.h>
#include "puzzle.h"

// Generates a "random" usp of the specified size from a particular
// distribution.  Uses lrand48().  May be slow.  Will not halt of USPs
// of that size do not exist.
puzzle * create_usp_greedy(int s, int k);

#endif
