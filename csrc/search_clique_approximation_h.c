#include <queue>
#include "puzzle.h"
#include "ExtensionGraph.hpp"
#include "searcher2.h"

heuristic_t clique_approximation_only_policy(puzzle * p, ExtensionGraph * eg){
  return CLIQUE_APPROXIMATION;
}

bool clique_approximation_vertex_reduce(unsigned long label_u, unsigned long degree_u, void * user_data){

  

}

int single_clique_approximation_h(puzzle * p, ExtensionGraph * eg){

  return 0;
}




std::priority_queue<heuristic_result> clique_approximation_h(puzzle * p, ExtensionGraph * eg){

  puzzle * p2 = extend_puzzle(p, 2);

  std::priority_queue<heuristic_result> hrq;
  ExtensionGraph neg(*eg);

  heuristic_data * hd = (heuristic_data *) malloc(sizeof(heuristic_data));

  hd->p = p2;
  hd->eg = &neg;
  hd->hr = hrq;

  eg->reduceVertices(clique_approximation_vertex_reduce, hd, false);
  free(hd);

  return hrq;

}
