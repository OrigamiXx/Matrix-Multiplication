#ifndef __SEARCH_HEURISTIC_TESTER_H__
#define __SEARCH_HEURISTIC_TESTER_H__

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "checker.h"
#include "puzzle.h"
#include "search_nullity.h"
#include "timing.h"

int time_check_heuristic(puzzle * p, bool skip[], bool ** skip_2d, int heuristic_num, double * time_ptr);

#endif