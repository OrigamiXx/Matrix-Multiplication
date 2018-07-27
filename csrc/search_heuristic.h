#include "searcher.h"
#include <queue>
#include "puzzle.h"
#include "Graph.hpp"

typedef enum heuristic_val {
  NULLITY,
  CLIQUE,
  INLINE_CLIQUE,
  INLINE_DEGREE,
  MIP_CLIQUE}
  heuristic_t;

typedef std::priority_queue<heuristic_result> (*search_heuristic_t)(puzzle *, ExtensionGraph *);

typedef heuristic_t (*heuristic_policy_t)(puzzle *, ExtensionGraph *);

int number_of_heuristics = 5;
const char * heuristic_names[] = {
  "nullity",
  "clique_approximation",
  "inline_vertex_degree",
  "inline_clique_approximation",
  "clique_mip"
};

search_heuristic_t heuristics[5] = {
  nullity_h,
  clique_h,
  inline_clique_h,
  inline_degree_h,
  mip_clique_h
}



