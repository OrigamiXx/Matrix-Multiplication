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
#include "search_MIP.h"
#include "search_timer.h"

using namespace std;

//======================================================================
//
//  Heuristic Header
//
//======================================================================

priority_queue<heuristic_result> * degree_h(puzzle * p, ExtensionGraph * eg);
priority_queue<heuristic_result> * greedy_clique_h(puzzle * p, ExtensionGraph * eg);
#ifdef __GUROBI_INSTALLED__
priority_queue<heuristic_result> * mip_clique_h(puzzle * p, ExtensionGraph * eg);
priority_queue<heuristic_result> * mip_search_h(puzzle * p, ExtensionGraph * eg);
#endif

// Data structures holding the heuristics that can be run during A*
// search.

#ifdef __GUROBI_INSTALLED__
extern const int number_of_heuristics = 4;
#else
extern const int number_of_heuristics = 2;
#endif

extern const char * heuristic_names[number_of_heuristics] = {
  "degree",
  "greedy clique"
  #ifdef __GUROBI_INSTALLED__
  ,
  "mip clique",
  "mip search"
  #endif
};

extern const search_heuristic_t heuristic_functions[] = {
  degree_h,
  greedy_clique_h
  #ifdef __GUROBI_INSTALLED__
  ,
  mip_clique_h,
  mip_search_h
  #endif
};

search_heuristic_t get_heuristic(heuristic_t ht){
  assert(ht < sizeof(heuristic_functions));
  return heuristic_functions[ht];
}

//======================================================================
//
//  Profiling
//
//======================================================================

typedef struct __heur_stat{

  unsigned long num;
  unsigned long num_search;

  unsigned long num_heur;
  unsigned long max_heur;
  unsigned long min_heur;
  unsigned long tot_heur;
  double ave_heur;
  double var_heur;

  double tot_time;
  double max_time;
  double min_time;
  double ave_time;
  double var_time;

} heur_stat;

#define MAX_S 100

static heur_stat stats[MAX_S];

bool reset_stats(){

  for (int i = 0; i < MAX_S; i++){
    stats[i].num = 0;
    stats[i].num_search = 0;
    stats[i].num_heur = 0;
    stats[i].max_heur = 0;
    stats[i].min_heur = 0x1L << 63;
    stats[i].tot_heur = 0;
    stats[i].ave_heur = 0.0;
    stats[i].var_heur = 0.0;

    stats[i].tot_time = 0.0;
    stats[i].max_time = 0.0;
    stats[i].min_time = 1e300;
    stats[i].ave_time = 0.0;
    stats[i].var_time = 0.0;

  }
  return true;
}

double inline diff_time(struct timespec start, struct timespec end){
  return ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)start.tv_sec + 1.0e-9*start.tv_nsec);
}

void record_stats(priority_queue<heuristic_result> * hrq, struct timespec start, struct timespec end, int s, unsigned long size){

  static bool force_init = reset_stats();
  (void)force_init;

  static struct timespec last_log = {0,0};
  static bool first_log = false;
  if (!first_log){
    clock_gettime(CLOCK_MONOTONIC, &last_log);
    first_log = true;
  }

  heur_stat * stat = &(stats[s]);

  priority_queue<heuristic_result> new_hrq(*hrq);

  stat -> num++;
  stat -> num_search += size;
  while(!new_hrq.empty()){
    heuristic_result res = new_hrq.top();
    new_hrq.pop();

    stat -> num_heur++;
    stat -> max_heur = MAX(stat -> max_heur, res.ideal);
    stat -> min_heur = MIN(stat -> min_heur, res.ideal);
    stat -> tot_heur += res.ideal;
    stat -> ave_heur = stat -> ave_heur * (1.0 - 1.0/stat -> num_heur)
      + res.ideal / (double)(stat -> num_heur);
    stat -> var_heur = stat -> var_heur * (1.0 - 1.0/stat -> num_heur)
      + (res.ideal - stat -> ave_heur) * (res.ideal - stat -> ave_heur) / (double)(stat -> num_heur);
  }

  double duration = diff_time(start, end);
  stat -> tot_time += duration;

  double time_per_heur = duration / size;
  stat -> max_time = MAX(time_per_heur, stat -> max_time);
  stat -> min_time = MIN(time_per_heur, stat -> min_time);
  stat -> ave_time = stat -> ave_time * ((stat -> num_search - size) / (double)(stat -> num_search))
    + time_per_heur * size / (double)(stat -> num_heur);
  stat -> var_time = stat -> var_time * ((stat -> num_search - size) / (double)(stat -> num_search))
    + (time_per_heur - stat -> ave_time) * (time_per_heur - stat -> ave_time)
    * size / (double)(stat -> num_heur);


  struct timespec curr_time = {0,0};
  clock_gettime(CLOCK_MONOTONIC, &curr_time);

  if (diff_time(last_log, curr_time) > 10){
    last_log = curr_time;
    fprint_search_stats(stdout);
  }

}

void fprint_search_stats(FILE * f){

  int i = 0;
  fprintf(f,"-----------------\n");
  fprintf(f,"   s,    num, num_search, num_heur, max_heur, min_heur, tot_heur,     ave_heur,     var_heur,     tot_time,     max_time,     min_time,     ave_time,     var_time\n");
  while(stats[i].num_search != 0){
    fprintf(f, "%4d, ", i);
    fprintf(f, "%6lu, ", stats[i].num);
    fprintf(f, "%10lu, ", stats[i].num_search);
    fprintf(f, "%8lu, ", stats[i].num_heur);
    fprintf(f, "%8lu, ", stats[i].max_heur);
    fprintf(f, "%8lu, ", stats[i].min_heur);
    fprintf(f, "%8lu, ", stats[i].tot_heur);
    fprintf(f, "%12.6f, ", stats[i].ave_heur);
    fprintf(f, "%12.6f, ", sqrt(stats[i].var_heur));

    fprintf(f, "%12.6f, ", stats[i].tot_time);
    fprintf(f, "%12.6f, ", stats[i].max_time);
    fprintf(f, "%12.6f, ", stats[i].min_time);
    fprintf(f, "%12.6f, ", stats[i].ave_time);
    fprintf(f, "%12.6f\n", sqrt(stats[i].var_time));
    i++;
  }

}


#define PROFILE(hrq, exp, p, size)						\
  {struct timespec __start = {0,0}, __end = {0,0}; clock_gettime(CLOCK_MONOTONIC, &__start); (q) = (exp); clock_gettime(CLOCK_MONOTONIC, &__end); record_stats((q), __start, __end, (p) -> s, (size));}




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
unsigned int generic_search(puzzle * p, ExtensionGraph * eg, heuristic_policy_t hp, unsigned int best){

  // Possibly update best seen.
  if (best < p -> s)
    printf("New best = %d\n", p -> s);
  best = MAX(best, p -> s);

  if (eg -> size() == 0)
    return best;

  // Determine and apply heuristic to all elements at frontier and
  // produce a priority queue.
  search_heuristic_t h = get_heuristic(hp(p, eg));
  std::priority_queue<heuristic_result> * q = NULL;
  PROFILE(q, (*h)(p, eg), p, eg -> size());

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
    if (best >= hr.ideal)
      break;

    // Recursively search and record new better value.
    best = MAX(best, generic_search(hr.p, hr.eg, hp, best));

    destroy_puzzle(hr.p);
    delete(hr.eg);

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
//  Global A* Search
//
//======================================================================

void print_frontier_dist(std::priority_queue<heuristic_result> * frontier){

  static struct timespec last_log = {0,0};
  static bool first_log = false;
  if (!first_log){
    clock_gettime(CLOCK_MONOTONIC, &last_log);
    first_log = true;
  }

  struct timespec curr_time = {0,0};
  clock_gettime(CLOCK_MONOTONIC, &curr_time);

  if (diff_time(last_log, curr_time) < 0.0001)
    return;

  last_log = curr_time;

  std::priority_queue<heuristic_result> tmp;

  unsigned long diff = 10;

  unsigned long max_ideal = frontier -> top().ideal;
  unsigned long counts[max_ideal + 1];
  bzero(counts, (max_ideal + 1) * sizeof(unsigned long));

  while(!frontier -> empty()){
    heuristic_result hr = frontier -> top();
    frontier -> pop();
    counts[hr.ideal]++;
    tmp.push(hr);

    if (hr.ideal + diff < max_ideal)
      break;
  }

  while(!tmp.empty()){
    heuristic_result hr = tmp.top();
    tmp.pop();
    frontier -> push(hr);
  }


  for(unsigned long i = 0; i < diff; i++)
    printf("%lu: %lu, ", max_ideal - i, counts[max_ideal - i]);
  printf("\n");


}

// Takes a puzzle width k and an admissible heuristic policy hp and
// performs A* search.  Returns the size of the largest width-k strong
// uniquely solvable puzzle.  Warning: Has side effect of clearing
// isomorph cache.
unsigned int global_search(int k, heuristic_policy_t hp){

  unsigned int best = 0;

  // Clear the isomorph cache.
  reset_isomorphs();

  // Stores entire search frontier.
  std::priority_queue<heuristic_result> frontier;

  puzzle * p = create_puzzle(0, k);
  ExtensionGraph * eg = new ExtensionGraph(p);

  // Add initial empty puzzle.
  search_heuristic_t h = get_heuristic(hp(p, eg));
  std::priority_queue<heuristic_result> * hrq = (*h)(p, eg);

  while(!hrq -> empty()){
    heuristic_result hr = hrq -> top();
    hrq -> pop();
    frontier.push(hr);
  }
  delete hrq;

  // Loop until frontier examined.
  while(!frontier.empty()){

    print_frontier_dist(&frontier);

    // Remove strongest candidate.
    heuristic_result hr = frontier.top();
    frontier.pop();

    // Update best.
    if (best < hr.p -> s)
      printf("\nNew best = %d\n", hr.p -> s);
    best = MAX(best, hr.p -> s);

    // We cannot do any better.
    if (hr.ideal <= best)
      break;

    // Compute children heuristics and add to frontier.
    h = get_heuristic(hp(hr.p, hr.eg));
    hrq = (*h)(hr.p, hr.eg);

    while(!hrq -> empty()){
      heuristic_result hr2 = hrq -> top();
      hrq -> pop();
      assert(hr2.ideal <= hr.ideal);
      if (hr2.ideal > best)
	frontier.push(hr2);
      else{
	destroy_puzzle(hr2.p);
	delete hr.eg;
      }
    }

    delete hrq;

    destroy_puzzle(hr.p);
    delete hr.eg;

  }

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

  puzzle * p2 = extend_puzzle(p, 1);

  // Helper function that sets heuristic result for every vertex in eg
  // to its degree.  No vertices are deleted.
  auto reduce_helper = [eg, p2, hrq](unsigned long index_u, unsigned long label_u, unsigned long degree_u) -> bool{

    p2 -> puzzle[p2 -> s - 1] = label_u;
    if (!have_seen_isomorph(p2, true)){

      puzzle * new_p = create_puzzle_copy(p2);
      ExtensionGraph * new_eg = new ExtensionGraph(*eg);
      new_eg->update(new_p);
      heuristic_result res = {.ideal = degree_u + p2 -> s, .p = new_p, .eg = new_eg};
      hrq->push(res);
    }
    return true;
  };

  // Computes the heuristic for each vertex.
  eg->reduceVertices(reduce_helper, false);

  destroy_puzzle(p2);

  return hrq;
}

priority_queue<heuristic_result> * greedy_clique_h(puzzle * p, ExtensionGraph * eg){

  // Priority queue of the heuristic results.
  priority_queue<heuristic_result> * hrq = new priority_queue<heuristic_result>();

  ExtensionGraph new_eg(*eg);

  unsigned long next_min = new_eg.getMinDegree();
  unsigned long curr_min = next_min;
  unsigned long max_degree = new_eg.getMaxDegree();
  unsigned long prev_label = 0;

  puzzle * p2 = extend_puzzle(p, 1);

  // Helper function that sets heuristic result for every vertex in eg
  // to its degree.  No vertices are deleted.
  auto reduce_helper =
    [eg, p2, hrq, &curr_min, &next_min, max_degree, &prev_label](unsigned long index_u, unsigned long label_u, unsigned long degree_u) -> bool
    {
      if (prev_label >= label_u){
	curr_min = next_min;
	next_min = max_degree;
      }
      prev_label = label_u;

      if (degree_u <= curr_min){
	p2 -> puzzle[p2 -> s - 1] = label_u;
	if (!have_seen_isomorph(p2, true)){

	  // Something is broken here, causes seg fault on global search.
	  puzzle * new_p = create_puzzle_copy(p2);
	  ExtensionGraph * new_eg = new ExtensionGraph(*eg);
	  new_eg->update(new_p);
	  heuristic_result res = {.ideal = curr_min + p2 -> s, .p = new_p, .eg = new_eg};
	  hrq->push(res);
	}
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

#ifdef __GUROBI_INSTALLED__
priority_queue<heuristic_result> * mip_clique_h(puzzle * p, ExtensionGraph * eg){

  // Priority queue of the heuristic results.
  priority_queue<heuristic_result> * hrq = new priority_queue<heuristic_result>();

  puzzle * p2 = extend_puzzle(p, 1);

  // Helper function that sets heuristic result for every vertex in eg
  // to its degree.  No vertices are deleted.
  auto reduce_helper = [p2,hrq,eg](unsigned long index_u, unsigned long label_u, unsigned long degree_u) -> bool{

    p2 -> puzzle[p2 -> s - 1] = label_u;
    if (!have_seen_isomorph(p2, true)){
      ExtensionGraph * new_eg = new ExtensionGraph(*eg);
      new_eg -> update(p2);
      unsigned long result = 0;
      if (new_eg->size() < 2)
	result = new_eg->size();
      else
	result = clique_mip_h(new_eg);

      assert(result <= new_eg->size());

      puzzle * new_p = create_puzzle_copy(p2);
      heuristic_result res = {.ideal = result + p2 -> s, .p = new_p, .eg = new_eg};
      hrq->push(res);
    }
    return true;
  };

  // Computes the heuristic for each vertex.
  eg->reduceVertices(reduce_helper, false);

  destroy_puzzle(p2);

  return hrq;
}



priority_queue<heuristic_result> * mip_search_h(puzzle * p, ExtensionGraph * eg){

  // Priority queue of the heuristic results.
  priority_queue<heuristic_result> * hrq = new priority_queue<heuristic_result>();

  puzzle * p2 = extend_puzzle(p, 1);

  // Helper function that sets heuristic result for every vertex in eg
  // to its degree.  No vertices are deleted.
  auto reduce_helper = [p2,hrq,eg](unsigned long index_u, unsigned long label_u, unsigned long degree_u) -> bool{

    p2 -> puzzle[p2 -> s - 1] = label_u;
    if (!have_seen_isomorph(p2, true)){
     ExtensionGraph * new_eg = new ExtensionGraph(*eg);
      new_eg -> update(p2);
      unsigned long result = 0;
      if (new_eg->size() < 2)
	result = new_eg->size();
      else
	result = mip_h(p2,new_eg);

      assert(result <= new_eg->size());

      puzzle * new_p = create_puzzle_copy(p2);
      heuristic_result res = {.ideal = result + p2 -> s, .p = new_p, .eg = new_eg};
      hrq->push(res);
    }
    return true;
  };

  // Computes the heuristic for each vertex.
  eg->reduceVertices(reduce_helper, false);

  destroy_puzzle(p2);

  return hrq;
}


#endif
