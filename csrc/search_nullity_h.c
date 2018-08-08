#include "searcher2.h"
#include "ExtensionGraph.hpp"
#include "puzzle.h"
#include "search_timer.h"
#include "Graph.hpp"

heuristic_t nullity_only_policy(puzzle * p, ExtensionGraph * eg){
  return NULLITY;
}

bool nullity_vertex_reduce(unsigned long label_u, unsigned long degree_u, void * user_data){

  printf("reducing\n");
  heuristic_data * hd = (heuristic_data *)user_data;
  printf("did some stuff with hd again\n"); 
  puzzle * hp = hd->p;
  printf("local pointer to puzzle\n");
  std::priority_queue<heuristic_result> * hrq = hd->hrq;
  printf("local pointer to the queue\n");
  ExtensionGraph * heg = hd->eg;
  printf("local pointer to the eg\n");
  // Set the last row of the puzzle to be the incoming row from the vertex reduction.
  if (hp -> s > 1 && label_u <= hp -> puzzle[hp -> s - 2])
    return false;
  
  hp->puzzle[hp->s-1] = label_u;
  
  if (IS_USP != check(hp)){
    return false;
  }

  heuristic_result res;
  res.value = label_u;
  printf("did some res stuff\n");
  puzzle * bp = create_puzzle(0, hp->k);
  printf("made a new puzzle\n");
  res.result = generic_search(bp, heg, nullity_only_policy, 0);
  printf("doing another search\n");

  hrq -> push(res);
  printf("reduced\n");
  return true;
}

  /*DISABLE_NEW_TIME(
    puzzle * bp = create_puzzle(0, p->k);
    hr = search(bp, eg);
    destroy_puzzle(bp);
  );*/
  

std::priority_queue<heuristic_result> * nullity_h(puzzle * p, ExtensionGraph * eg){
  printf("heuristic start\n");

  puzzle * p2 = extend_puzzle(p, 1);
  printf("pointer is going to %x\n", &p2);
  printf("made a puzzle\n");
  std::priority_queue<heuristic_result> * hrq = new std::priority_queue<heuristic_result>();
  printf("made a queue\n");
  ExtensionGraph neg(*eg);
  printf("made an eg\n");

  heuristic_data hd;
  printf("got some hd\n");
  hd.p = p2;
  hd.eg = &neg;
  hd.hrq = hrq; 
  printf("set some hd details\n");

  eg->reduceVertices(nullity_vertex_reduce, &hd, false); 
  printf("heuristic end\n");
  destroy_puzzle(p2);

  return hrq; 
}
