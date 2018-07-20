#ifndef __SEARCH_HEURISTIC_TESTER_H__
#define __SEARCH_HEURISTIC_TESTER_H__

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "checker.h"
#include "puzzle.h"
#include "timing.h"
#include "search_heuristic_tester.h"
#include "searcher.h"
#include "timing.h"

struct heuristic_timed {
  double t_max;
  double t_min;
  double t_total;
  int num_trials;
};

int time_check_heuristic(puzzle * p, bool skip[], bool ** skip_2d, int heuristic_num, double * time_ptr);

#endif