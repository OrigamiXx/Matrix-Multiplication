/*
 * Module implementing several methods for combining USPs into larger
 * USPs.
 *
 * Author: Matt.
 */

#ifndef __CONSTRUCT_H__
#define __CONSTRUCT_H__

#include <stdio.h>
#include <stdlib.h>
#include "puzzle.h"




// Public puzzle creation functions.
puzzle * create_random_twist(puzzle * p1, puzzle * p2, int iter);
puzzle * create_full_twist(puzzle * p1, puzzle * p2);  
puzzle * create_puzzle_product(puzzle * p1, puzzle * p2);
puzzle * create_puzzle_paired(puzzle * p1);

// "Private" Functions
void populate(int iter, int max_k, bool display);
void print_graph(int k);

#endif
