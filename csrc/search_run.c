#include <stdio.h>
#include <stdlib.h>

#include "searcher.h"
#include "constants.h"
#include "checker.h"
#include "puzzle.h"

extern cumulative_counts * heuristic_details;
extern int heuristic_details_count;
extern const char * heuristic_names[];

int main(int argc, char ** argv) {
  if (argc != 4){
    fprintf(stderr, "usage: search_run <s> <k> <heuristic type>\n");
    fprintf(stderr, " s = the value of s to stop searching at ; k = k to search at\n");
    fprintf(stderr, " Heuristic types:\n");
    fprintf(stderr, "  # 0 = nullity\n");
    fprintf(stderr, "  # 1 = clique\n");
    fprintf(stderr, "  # 2 = inline degree\n");
    fprintf(stderr, "  # 3 = inline clique\n");
    fprintf(stderr, "  # 4 = clique mip\n");
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
  puzzle_row max_index = ipow2(3,start_p->k);
  
  bool ** init_graph = make_graph_from_puzzle(start_p, init_skip, 0, max_index, -1, -1);

  int result;

  int buffer_size = 75;
  char log_name[buffer_size];
  snprintf(log_name, buffer_size, "search_run <max_s= %d> <k= %d> <heuristic= %s>", s, k, heuristic_names[h_type]);

  init_log(log_name);

  switch(h_type) {
    case 0:
    case 1:
    case 4:
      result = search(start_p, init_skip, 0, 0, 0, h_type, s, true, 0);
      break;
    case 2:
    case 3:
      result = inline_search(start_p, init_graph, init_skip, 0, 0, 0, h_type, s, 0);
      break;
  }


  destroy_puzzle(start_p);

  log_current_results(true);

  printf("Max rows found: %d\n", result);


  return 0;
}
