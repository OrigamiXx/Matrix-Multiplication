/*
 * Module implementing several methods for combining USPs into larger
 * USPs.
 *
 * Author: Matt.
 */

#ifndef __USP_CONSTRUCT_H__
#define __USP_CONSTRUCT_H__

#include <stdio.h>
#include <stdlib.h>
#include "puzzle.h"

puzzle * create_random_twist(puzzle * p1, puzzle * p2, int iter);
puzzle * create_full_twist(puzzle * p1, puzzle * p2);  
puzzle * create_puzzle_product(puzzle * p1, puzzle * p2);
puzzle * create_puzzle_paired(puzzle * p1);

#endif
