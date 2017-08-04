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

// Runs checker on p and returns result, return time taken in second through time_ptr.
int time_check(int (* checker)(puzzle *), puzzle * p, double * time_ptr);

#endif
