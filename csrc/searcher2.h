#pragma once

#include <queue>
#include "puzzle.h"
#include "ExtensionGraph.hpp"

typedef enum heuristic_val {
  NULLITY,
  CLIQUE,
  INLINE_CLIQUE,
  INLINE_DEGREE,
  MIP_CLIQUE
} heuristic_t;


typedef struct heuristic_result {
  int result;
  puzzle_row value;

  bool operator<(const heuristic_result & rhs) const
  {
    return result < rhs.result;
  }
} heuristic_result;

// Heuristic definitions must follow this function outline.
typedef std::priority_queue<heuristic_result> (*search_heuristic_t)(puzzle *, ExtensionGraph *);
// Heuristic policies must follow this function outline.
typedef heuristic_t (*heuristic_policy_t)(puzzle *, ExtensionGraph *);


heuristic_t generic_policy(puzzle * p, ExtensionGraph * eg);
search_heuristic_t get_heuristic(heuristic_t ht);

// This function can be called externally
int generic_search(int k, heuristic_policy_t hp);
int generic_search(puzzle * p, ExtensionGraph * eg, heuristic_policy_t hp, int best);


typedef struct heuristic_data {

  puzzle * p;
  ExtensionGraph * eg;
  std::priority_queue<heuristic_result> hr;

} heuristic_data;

std::priority_queue<heuristic_result> individual_heuristics(puzzle * p, ExtensionGraph * eg, heuristic_t ht);

// Puzzle will come into heuristics extended by 0 as default
// All heuristic definitions go below here.
// NULLITY => nullity_h
heuristic_t nullity_policy(puzzle * p, ExtensionGraph * eg);
int single_nullity_h(puzzle * p, ExtensionGraph * eg);
bool nullity_vertex_reduce(unsigned long label_u, unsigned long degree_u, void * user_data);
std::priority_queue<heuristic_result> nullity_h(puzzle *p, ExtensionGraph * eg);

// CLIQUE_APPROXIMATION => clique_approximation_h
int single_clique_approximation_h(puzzle *, ExtensionGraph *);
std::priority_queue<heuristic_result> clique_approximation_h(puzzle *, ExtensionGraph *);
