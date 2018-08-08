#include "searcher2.h"

heuristic_t current_heuristic = NULLITY;

// This should be in the search_run file.
heuristic_t search_run_policy(puzzle * p, ExtensionGraph * eg){
  return current_heuristic;
}

int main(int argc, char ** argv){

  int res = generic_search(3, search_run_policy);

  printf("hello we here %d \n", res);

}
