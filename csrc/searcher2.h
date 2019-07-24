#pragma once

#include <queue>
#include "puzzle.h"
#include "ExtensionGraph.hpp"

typedef enum heuristic_val {
  VERTEX_DEGREE,
  GREEDY_CLIQUE,
  MIP_CLIQUE,
  MIP_SEARCH
} heuristic_t;

typedef struct heuristic_result {
  unsigned long ideal;
  puzzle * p;
  ExtensionGraph * eg;

  bool operator<(const heuristic_result & rhs) const
  {
    return ideal < rhs.ideal || (ideal == rhs.ideal && p -> s < rhs.p -> s);
  }
} heuristic_result;

// Heuristic definitions must follow this function outline.
typedef std::priority_queue<heuristic_result> *(*search_heuristic_t)(puzzle *, ExtensionGraph *);
// Heuristic policies must follow this function outline.
typedef heuristic_t (*heuristic_policy_t)(puzzle *, ExtensionGraph *);

// Takes a puzzle width k and an admissible heuristic policy hp and
// performs A* search.  Returns the size of the largest width-k strong
// uniquely solvable puzzle.  Warning: Has side effect of clearing
// isomorph cache.
int generic_search(int k, heuristic_policy_t hp);

// Takes a puzzle width k and an admissible heuristic policy hp and
// performs A* search.  Returns the size of the largest width-k strong
// uniquely solvable puzzle.  Warning: Has side effect of clearing
// isomorph cache.
unsigned int global_search(int k, heuristic_policy_t hp);

void fprint_search_stats(FILE * f);

void record_stats(priority_queue<heuristic_result> * hrq, struct timespec start, struct timespec end, int s, unsigned long size);

priority_queue<heuristic_result> * degree_h(puzzle * p, ExtensionGraph * eg);
priority_queue<heuristic_result> * greedy_clique_h(puzzle * p, ExtensionGraph * eg);
#ifdef __GUROBI_INSTALLED__
priority_queue<heuristic_result> * mip_clique_h(puzzle * p, ExtensionGraph * eg);
priority_queue<heuristic_result> * mip_search_h(puzzle * p, ExtensionGraph * eg);
#endif


// Data structures holding the heuristics that can be run during A*
// search.
extern const int number_of_heuristics;
extern const char * heuristic_names[];
extern const search_heuristic_t heuristic_functions[];
