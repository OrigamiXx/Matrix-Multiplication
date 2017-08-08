/*
 * Module implementing several non-trivial heursitics for checking
 * whether a given puzzle is a strong USP.
 *
 * Author: Matt.
 */

#ifndef __HEURISTIC_H__
#define __HEURISTIC_H__

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
#include "usp.h"
using namespace std;

/*
 * A specialized function that determines whether any pair of rows
 * prevent an s-by-k from being a strong USP.  Returns NOT_USP or
 * UNDET_USP.
 */
check_t heuristic_row_pairs(puzzle * p);

/*
 * A specialized function that determines whether any triple of rows
 * prevent an s-by-k from being a strong USP.  Returns NOT_USP or
 * UNDET_USP.
 */
check_t heuristic_row_triples(puzzle * p); 

/*
 * Polytime heuristic that checks whether 2DM exists for each of the
 * projected faces of the 3DM instance.  Returns UNDET_USP or IS_USP.
 * XXX - Almost never returns IS_USP.
 */
check_t heuristic_2d_matching(puzzle * p);

/*
 * Heuristically precheck puzzle via random and greedy approaches
 * Returns IS_USP if puzzle is a strong USP.  Returns NOT_USP if
 * puzzle is not a strong USP.  Returns UNDET_USP if the function has
 * not determined the puzzle is a strong USP.
 */
check_t heuristic_random(puzzle * p);

/*
 * Returns NOT_USP iff it finds a witness that puzzle is not a strong
 * USP.  UNDET_USP indicates the search was inconclusive.  Attempts to
 * generate a witness by greedily selecting a layer from among those
 * with fewest remaining edges and then uniformly selects an edge from
 * that layer.  This repeats until a witness is found, or no progress
 * can be made.  This process repeats for some specified number of
 * iterations.  There is no benefit to reorder_witnesses() be called
 * before this.  Requires s <= 31.
 */
check_t heuristic_greedy(puzzle * p);

#endif
