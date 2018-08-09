#include <queue>
#include "puzzle.h"
#include "searcher2.h"
#include "search_timer.h"
#include "Graph.hpp"
#include "ExtensionGraph.hpp"

heuristic_t vertex_degree_only_policy(puzzle * p, ExtensionGraph * eg){
  return VERTEX_DEGREE;
}

bool vertex_degree_reduce(unsigned long label_u, unsigned long degree_u, void * user_data){
  
  if (degree_u < 0)
    return false;

  heuristic_data * hd = (heuristic_data *)user_data;
  std::priority_queue<heuristic_result> * hrq = hd->hrq;

  heuristic_result res;
  res.value = label_u;
  res.result = degree_u;

  hrq->push(res);

  return true;
}

std::priority_queue<heuristic_result> * vertex_degree_h(puzzle * p, ExtensionGraph * eg){

  std::priority_queue<heuristic_result> * hrq = new std::priority_queue<heuristic_result>();

  heuristic_data hd;
  hd.p = p;
  hd.eg = eg;
  hd.hrq = hrq;

  eg->reduceVertices(vertex_degree_reduce, &hd, false);


  return hrq;
}
