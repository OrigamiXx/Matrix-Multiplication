#ifndef __CLIQUE_TO_MIP_H__
#define __CLIQUE_TO_MIP_H__

#include <stdio.h>
#include <stdlib.h>
#include "puzzle.h"
#include "constants.h"
#include "gurobi_c++.h"

int max_clique_mip(bool ** graph, puzzle_row graph_max_index);

#endif