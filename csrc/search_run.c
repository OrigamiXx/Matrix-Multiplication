#include <stdio.h>
#include <stdlib.h>

#include "searcher.h"
#include "constants.h"
#include "checker.h"
#include "puzzle.h"

int main(int argc, char ** argv) {

  if (argc != 4){
    fprintf(stderr, "usage: search_nullity <s> <k> <heuristic type>\n");
    fprintf(stderr, "Heuristic types:\n # 0 = nullity\n # 1 = clique\n");
    fprintf(stderr, ">> s currently unused <<\n");
    return -1;
  }

  int s = atoi(argv[1]);
  int k = atoi(argv[2]);
  int h_type = atoi(argv[3]);

  if (h_type < 0 || h_type > 1) {
    fprintf(stderr, "Invalid heuristic type used\n");
    return -1;
  }

  puzzle * start_p = create_puzzle(0, k);

  bool init_skip[start_p->max_row];
  bzero(init_skip, sizeof(init_skip));

  int result = search(start_p, init_skip, 0, 0, 0, h_type);

  destroy_puzzle(start_p);

  printf("Max rows found: %d\n", result);

}