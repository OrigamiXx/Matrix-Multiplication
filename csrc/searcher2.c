#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <queue>
#include <string>
#include <time.h>

#include "constants.h"
#include "checker.h"
#include "puzzle.h"
#include "timing.h"
#include "clique_to_mip.h"
#include "canonization.h"

#include "searcher2.h"

#include "search_timer.h"

const int number_of_heuristics = 5;
const char * heuristic_names[number_of_heuristics] = {
  "nullity",
  "clique",
  "inline_clique",
  "vertex_degree",
  "mip_clique"
};

const search_heuristic_t heuristic_functions[] = {
  
  nullity_h,
  nullity_h,
  nullity_h,
  vertex_degree_h
  /*inline_clique_approximation_h,
  clique_mip_h*/
  
};

// This should be in the search_run file.
heuristic_t generic_policy(puzzle * p, ExtensionGraph * eg){
  return NULLITY;
}

search_heuristic_t get_heuristic(heuristic_t ht){
  assert(ht < sizeof(heuristic_functions));
  return heuristic_functions[ht]; 
}

int generic_search(int k, heuristic_policy_t hp){

  int best = 0;

  // Create the initial puzzle and graph to search on.
  puzzle * p = create_puzzle(0, k);
  ExtensionGraph eg(p);

  // Start searching.
  /*TIME(
      best = generic_search(p, &eg, generic_policy, best),
      p->s,
      hp(p, eg),
      "SEARCH"
      );*/
  
  //printf("-1");
  best = generic_search(p, &eg, hp, best);
  destroy_puzzle(p);

  return best;
}

int generic_search(puzzle * p, ExtensionGraph * eg, heuristic_policy_t hp, int best){
  //printf("current best is %d\n", best);
  
  //if (have_seen_isomorph(p, true)) return best;

  // Copy then extend the graph.
  ExtensionGraph new_eg(*eg); // Should this be timed?
  /*TIME(
    new_eg.update(p2);,
    p2->s,
    hp(p, eg),
    "GRAPH"
    );*/
  new_eg.update(p);

  assert(new_eg.size() < eg->size() || p->s == 0);
  assert(new_eg.size() >= 0 && new_eg.size() <= p->max_row);
  assert(p->s <= p->max_row);
  
  
  //printf("0");
  heuristic_t h_type = hp(p, &new_eg);
  //printf("1");
  search_heuristic_t h = get_heuristic(h_type);
  //printf("2");
  std::priority_queue<heuristic_result> * q = (*h)(p, &new_eg);
  //printf("3");
  //printf("made it at least here\n");
  if (p->s > best) {
    printf("best updated to %d\n", p->s);
  }
  best = MAX(best, p -> s);
  
 
  puzzle * p2 = extend_puzzle(p, 1);

  while(!q -> empty()){
    //printf("popped one\n");  
    heuristic_result hr = q -> top();
    q -> pop();
    
    // Max expected from heuristic is (new puzzle size + heuristic result).
    // If our best is already better, there's not much point searching. 
    if (best >= hr.result + p2->s)
      break;
    
    // Update the last row of p2 accordingly.
    p2->puzzle[(p2->s)-1] = hr.value;
    
    // Recursively search.
    /*TIME(
      best = max(best, generic_search(p2, new_eg, hp, best));,
      p2->s,
      hp(p, eg),
      "SEARCH"
      );*/
    //printf("about to search again\n");
    best = MAX(best, generic_search(p2, &new_eg, hp, best));
    //printf("searched again\n");
  }

  destroy_puzzle(p2);
  delete q;
  //printf("returning %d as best\n", best);
  return best;
}


