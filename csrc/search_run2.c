#include "searcher2.h"

heuristic_t current_heuristic = VERTEX_DEGREE;

heuristic_t search_run_policy(puzzle * p, ExtensionGraph * eg){
  return current_heuristic;
}

int main(int argc, char ** argv){

  // TODO: make it use command line arguments for size and policy
  int res = generic_search(4, search_run_policy);

  printf("hello we here %d \n", res);

}
