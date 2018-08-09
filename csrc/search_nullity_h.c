#include "searcher2.h"
#include "ExtensionGraph.hpp"
#include "puzzle.h"
#include "search_timer.h"
#include "Graph.hpp"

heuristic_t nullity_only_policy(puzzle * p, ExtensionGraph * eg){
  return NULLITY;
}

bool nullity_vertex_reduce(unsigned long label_u, unsigned long degree_u, void * user_data){
  
  
  heuristic_data * hd = (heuristic_data *)user_data;
  puzzle * hp = hd->p;
  std::priority_queue<heuristic_result> * hrq = hd->hrq;
  ExtensionGraph * heg = hd->eg;
  // Set the last row of the puzzle to be the incoming row from the vertex reduction.
  if (hp -> s > 1 && label_u <= hp -> puzzle[hp -> s - 2])
    return false;
  
  hp->puzzle[hp->s-1] = label_u;
  assert(hp->s >= 1 && hp->s <= hp->max_row);
  if (IS_USP != check(hp)){
    return false;
  }

  heuristic_result res;
  res.value = label_u;
  puzzle * bp = create_puzzle(1, hp->k);
  bp->puzzle[0] = label_u;
  res.result = generic_search(bp, heg, nullity_only_policy, 0);

  hrq -> push(res);
  return true;
}

  /*DISABLE_NEW_TIME(
    puzzle * bp = create_puzzle(0, p->k);
    hr = search(bp, eg);
    destroy_puzzle(bp);
  );*/
  

std::priority_queue<heuristic_result> * nullity_h(puzzle * p, ExtensionGraph * eg){
  puzzle * p2 = extend_puzzle(p, 1);
  std::priority_queue<heuristic_result> * hrq = new std::priority_queue<heuristic_result>();

  heuristic_data hd;
  hd.p = p2;
  hd.eg = eg;
  hd.hrq = hrq; 

  eg->reduceVertices(nullity_vertex_reduce, &hd, false); 
  destroy_puzzle(p2);

  return hrq; 
}
