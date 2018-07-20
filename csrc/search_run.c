#include <stdio.h>
#include <stdlib.h>

#include "searcher.h"
#include "constants.h"
#include "checker.h"
#include "puzzle.h"

extern cumulative_counts * heuristic_details;
extern int heuristic_details_count;

int main(int argc, char ** argv) {
  if (argc != 4){
    fprintf(stderr, "usage: search_nullity <s> <k> <heuristic type>\n");
    fprintf(stderr, "  s = the value of s to stop searching at ; k = k to search at");
    fprintf(stderr, "  Heuristic types:\n # 0 = nullity\n # 1 = clique\n # 2 = inline degree\n # 3 = inline clique\n # 4 = clique mip\n");
    return -1;
  }

  int s = atoi(argv[1]);
  int k = atoi(argv[2]);
  int h_type = atoi(argv[3]);

  if (h_type < 0 || h_type > 4) {
    fprintf(stderr, "Invalid heuristic type used\n");
    return -1;
  }

  puzzle * start_p = create_puzzle(0, k);

  bool init_skip[start_p->max_row];
  bzero(init_skip, sizeof(init_skip));

  bool ** init_graph = make_graph_from_puzzle(start_p, init_skip, 0, ipow2(3, start_p->k)-1, ipow2(3, start_p->k), -1, -1);

  int result;

  switch(h_type) {
    case 0:
    case 1:
    case 4:
      result = search(start_p, init_skip, 0, 0, 0, h_type, s, true);
      break;
    case 2:
    case 3:
      result = inline_search(start_p, init_graph, init_skip, 0, 0, 0, h_type, s);
      break;
  }


  destroy_puzzle(start_p);

  printf("Max rows found: %d\n", result);


  return 0;
}