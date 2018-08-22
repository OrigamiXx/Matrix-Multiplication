// Implementation of A* search for strong uniquely solvable puzzles.
// Includes a number of heuristics and policies for performing the
// search.

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

using namespace std;

priority_queue<heuristic_result> * degree_h(puzzle * p, ExtensionGraph * eg);
priority_queue<heuristic_result> * greedy_clique_h(puzzle * p, ExtensionGraph * eg);
priority_queue<heuristic_result> * mip_clique_h(puzzle * p, ExtensionGraph * eg);

// Data structures holding the heuristics that can be run during A*
// search.
const int number_of_heuristics = 3;//5;
const char * heuristic_names[number_of_heuristics] = {
  //"nullity",
  //"clique",
  //"inline_clique",
  "degree",
  "greedy clique"
  "mip clique"
  //"mip_clique"
};

const search_heuristic_t heuristic_functions[] = {
  
  //nullity_h,
  //nullity_h,
  //nullity_h,
  degree_h,
  greedy_clique_h,
  mip_clique_h
  /*inline_clique_approximation_h,
  clique_mip_h*/
  
};

search_heuristic_t get_heuristic(heuristic_t ht){
  assert(ht < sizeof(heuristic_functions));
  return heuristic_functions[ht]; 
}

//======================================================================
//
//  Generic A* Search
//
//======================================================================

// Takes a puzzle p with corresponding ExtensionGraph eg, an
// admissible heuristic policy hp and the largest puzzle best seen so
// far.  Performs A* search from this puzzle and using this policy.
// Returns the size of the largest strong uniquely solvable puzzle
// that can be extended from p.  Warning: Updates and uses isomorph
// cache.
int generic_search(puzzle * p, ExtensionGraph * eg, heuristic_policy_t hp, int best){

  // Possibly update best seen.
  if (best < p -> s)
    printf("New best = %d\n", p -> s);
  best = MAX(best, p -> s);
  


  // Determine and apply heuristic to all elements at frontier and
  // produce a priority queue.
  search_heuristic_t h = get_heuristic(hp(p, eg));
  std::priority_queue<heuristic_result> * q = (*h)(p, eg);

  // Iterate over all elements of the frontier in the priority queue
  // in decreasing order of heuristic value.
  puzzle * p2 = extend_puzzle(p, 1);
  
  while(!q -> empty()){

    // Remove strongest candidate.
    heuristic_result hr = q -> top();
    q -> pop();
    
    // Max expected from heuristic is (new puzzle size + heuristic
    // result).  If our best is already better, there is no point in
    // continue to search this frontier.
    if (best >= hr.result + p2->s)
      break;
    
    // Update the last row of p2.
    p2->puzzle[(p2->s)-1] = hr.value;

    // Skip if we've already searched on an isomorph of this puzzle.
    if (have_seen_isomorph(p2, true)) 
      continue;
    
    // Copy then extend the graph.
    ExtensionGraph new_eg(*eg);
    
    // Update the extension graph because the puzzle was just updated
    // (unless it's the top level puzzle).
    new_eg.update(p2);

    // Sanity checks
    assert(new_eg.size() < eg->size());
    assert(new_eg.size() >= 0 && new_eg.size() <= p2->max_row);
    
    // Recursively search and record new better value.
    best = MAX(best, generic_search(p2, &new_eg, hp, best));
  
  }

  // Clean up.
  destroy_puzzle(p2);
  delete q;

  return best;
}


// Takes a puzzle width k and an admissible heuristic policy hp and
// performs A* search.  Returns the size of the largest width-k strong
// uniquely solvable puzzle.  Warning: Has side effect of clearing
// isomorph cache.
int generic_search(int k, heuristic_policy_t hp){

  int best = 0;

  // Clear the isomorph cache.
  reset_isomorphs();
  
  // Create the initial puzzle and graph to search on.
  puzzle * p = create_puzzle(0, k);
  ExtensionGraph eg(p);

  // Perform the search.
  best = generic_search(p, &eg, hp, best);

  //Clean up.
  destroy_puzzle(p);

  // Clear the isomorph cache.
  reset_isomorphs();
  
  return best;
}

//======================================================================
//
//  A* Search Heuristics
//
//======================================================================

// Returns a priority queue with the heuristic results which is the
// degree of each vertex in eg.  Assumes graph eg corresponds to
// puzzle p. Doesn't modify p or eg.
priority_queue<heuristic_result> * degree_h(puzzle * p, ExtensionGraph * eg){

  // Priority queue of the heuristic results.
  priority_queue<heuristic_result> * hrq = new priority_queue<heuristic_result>();

  // Helper function that sets heuristic result for every vertex in eg
  // to its degree.  No vertices are deleted.
  auto reduce_helper = [p,hrq](unsigned long index_u, unsigned long label_u, unsigned long degree_u) -> bool{
    heuristic_result res = {.result = degree_u, .value = label_u};
    hrq->push(res);
    return true;
  };

  // Computes the heuristic for each vertex.
  eg->reduceVertices(reduce_helper, false);

  return hrq;
}

priority_queue<heuristic_result> * greedy_clique_h(puzzle * p, ExtensionGraph * eg){

  // Priority queue of the heuristic results.
  priority_queue<heuristic_result> * hrq = new priority_queue<heuristic_result>();

  ExtensionGraph new_eg(*eg);
  
  int curr_min = -1;
  int next_min = new_eg.getMinDegree();
  int max_degree = new_eg.getMaxDegree();
  unsigned long prev_label = 0;
  
  // Helper function that sets heuristic result for every vertex in eg
  // to its degree.  No vertices are deleted.
  auto reduce_helper =
    [p, hrq, &curr_min, &next_min, max_degree, &prev_label](unsigned long index_u, unsigned long label_u, unsigned long degree_u) -> bool
    {
      if (curr_min < 0 || prev_label >= label_u){
	curr_min = next_min;
	next_min = max_degree;
      }
      prev_label = label_u;
      
      if (degree_u <= curr_min){
	heuristic_result res = {.result = curr_min, .value = label_u};
	hrq->push(res);
	return false;
      } else {
	next_min = MIN(degree_u, next_min);
	return true;
      }
    };

  // Computes the heuristic for each vertex.
  new_eg.reduceVertices(reduce_helper, true);

  return hrq;
  
}


priority_queue<heuristic_result> * mip_clique_h(puzzle * p, ExtensionGraph * eg){

  // Priority queue of the heuristic results.
  priority_queue<heuristic_result> * hrq = new priority_queue<heuristic_result>();

  puzzle * p2 = extend_puzzle(p, 1);  
  
  // Helper function that sets heuristic result for every vertex in eg
  // to its degree.  No vertices are deleted.
  auto reduce_helper = [p2,hrq,eg](unsigned long index_u, unsigned long label_u, unsigned long degree_u) -> bool{

    ExtensionGraph new_eg(*eg);
    p2 -> puzzle[p2 -> s - 1] = label_u;
    new_eg.update(p2);
    unsigned long result = 0;
    if (new_eg.size() < 2)
      result = new_eg.size();
    else
      result = max_clique_mip(&new_eg);
    
    heuristic_result res = {.result = result, .value = label_u};
    hrq->push(res);
    return true;
  };

  // Computes the heuristic for each vertex.
  eg->reduceVertices(reduce_helper, false);
  
  destroy_puzzle(p2);
  
  return hrq;
}
