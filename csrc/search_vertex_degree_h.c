#include <queue>
#include "puzzle.h"
#include "searcher2.h"
#include "search_timer.h"
#include "Graph.hpp"
#include "ExtensionGraph.hpp"

heuristic_t vertex_degree_only_policy(puzzle * p, ExtensionGraph * eg){
  return VERTEX_DEGREE;
}


