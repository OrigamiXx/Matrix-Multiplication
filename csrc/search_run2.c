#include "searcher2.h"

heuristic_t current_heuristic = VERTEX_DEGREE;

heuristic_t search_run_policy(puzzle * p, ExtensionGraph * eg){
  return current_heuristic;
}

int main(int argc, char ** argv){

  int k = 5;
  int s = generic_search(k, search_run_policy);

  printf("Found a (%d, %d) strong USP.\n", s, k);

  
}
