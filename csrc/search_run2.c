#include "searcher2.h"

heuristic_t current_heuristic = MIP_CLIQUE;//VERTEX_DEGREE;//GREEDY_CLIQUE;//VERTEX_DEGREE;

heuristic_t search_run_policy(puzzle * p, ExtensionGraph * eg){

  if (p -> s > 10){
    return MIP_CLIQUE;
  } else {
    return VERTEX_DEGREE;
  }

}

int main(int argc, char ** argv){

  int k = 2;
  if (argc > 1)
    k = atoi(argv[1]);

  int s = generic_search(k, search_run_policy);

  printf("Found a (%d, %d) strong USP.\n", s, k);

  
}
