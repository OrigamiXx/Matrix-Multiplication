#include "ExtensionGraph.hpp"
#include "puzzle.h"
#include "search_MIP.h"

int main(int argc, char * argv[]){

  //  puzzle * init_p = create_puzzle_from_string((char *)"111112\n231231\n123123\n321113\n333222\n");
  puzzle * init_p = create_puzzle_from_string((char *)"1123\n");
  //puzzle * init_p = create_puzzle(0, 4);
  ExtensionGraph eg(init_p);

  #ifdef __GUROBI_INSTALLED__
  int max_clique = mip_h(init_p, &eg) + init_p -> s;

  printf("Best size = %d\n", max_clique);
  #else
  assert("Gurobi not installed\n" == 0);
  #endif
  
  return 0;
}
