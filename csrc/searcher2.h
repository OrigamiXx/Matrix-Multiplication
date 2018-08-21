#pragma once

#include <queue>
#include "puzzle.h"
#include "ExtensionGraph.hpp"

typedef enum heuristic_val {
  NULLITY,
  CLIQUE,
  INLINE_CLIQUE,
  VERTEX_DEGREE,
  MIP_CLIQUE
} heuristic_t;



typedef struct heuristic_result {
  unsigned long result;
  puzzle_row value;

  bool operator<(const heuristic_result & rhs) const
  {
    return result < rhs.result;
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

// Puzzle will come into heuristics extended by 0 as default
// All heuristic definitions go below here.
// NULLITY => nullity_h
heuristic_t nullity_policy(puzzle * p, ExtensionGraph * eg);
int single_nullity_h(puzzle * p, ExtensionGraph * eg);
bool nullity_vertex_reduce(unsigned long label_u, unsigned long degree_u, void * user_data);
std::priority_queue<heuristic_result> * nullity_h(puzzle *p, ExtensionGraph * eg);

// CLIQUE_APPROXIMATION => clique_approximation_h
int single_clique_approximation_h(puzzle * p, ExtensionGraph * eg);
std::priority_queue<heuristic_result> clique_approximation_h(puzzle *, ExtensionGraph *);

heuristic_t vertex_degree_only_policy(puzzle * p, ExtensionGraph * eg);
bool vertex_degree_reduce(unsigned long label_u, unsigned long degree_u, void * user_data);
std::priority_queue<heuristic_result> * vertex_degree_h(puzzle * p, ExtensionGraph * eg);
