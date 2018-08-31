#ifndef __CLIQUE_TO_MIP_H__
#define __CLIQUE_TO_MIP_H__

#include <stdio.h>
#include <stdlib.h>
#include "puzzle.h"
#include "constants.h"
#include "gurobi_c++.h"
#include "ExtensionGraph.hpp"

int clique_mip_h(ExtensionGraph * eg);
int clique_mip_h(puzzle * p);

#endif
