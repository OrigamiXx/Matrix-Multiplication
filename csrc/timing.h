/*
 * Module implementing several methods for combining USPs into larger
 * USPs.
 *
 * Author: Matt.
 */

#ifndef __TIMING_H__
#define __TIMING_H__

#include <stdio.h>
#include <stdlib.h>

#include "puzzle.h"
#include "checker.h"


// Runs checker on p and returns result, return time taken in second through time_ptr.
check_t time_check(checker_t checker, puzzle * p, double * time_ptr);

#endif
