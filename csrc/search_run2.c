#include "searcher2.h"

heuristic_t search_run_policy(puzzle * p, ExtensionGraph * eg){

  /*
  if (p -> s <= 4){
    return MIP_CLIQUE;
  } else {
    return VERTEX_DEGREE;
  }
  */
  /*
  if (p -> s <= 8){
    return VERTEX_DEGREE;
  }
  return MIP_SEARCH;
  */
  return VERTEX_DEGREE;
  /*
  if (p -> s <= 7)
    return MIP_CLIQUE;
  else if (p -> s <= 12)
    return GREEDY_CLIQUE;
  else
    return VERTEX_DEGREE;
  */
  //return MIP_CLIQUE;
  //return MIP_SEARCH;

}

int main(int argc, char ** argv){

  int k = 2;
  if (argc > 1)
    k = atoi(argv[1]);

  int s = generic_search(k, search_run_policy);

  printf("Found a (%d, %d) strong USP.\n", s, k);

  fprint_search_stats(stdout);
  
  return 0;
}
