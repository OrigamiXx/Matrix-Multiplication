#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "checker.h"
#include "puzzle.h"

// Simple search to verify that there are local maxima for k >= 5
// k          =  5   6   7   8
// local max  =  7   9  12  18
// best max   =  8  14  21  30
// best upper =  8  30  55 105

int main(int argc, char *argv[])
{

  if (argc != 2){
    fprintf(stderr,"usage: search_really_greedy <k>\n");
    return -1;
  }

  int k = atoi(argv[1]);

  puzzle * p = create_puzzle(0, k);
  puzzle * p2 = NULL;

  bool progress = true;
  while (progress){
    progress = false;
    
    p2 = extend_puzzle(p, 1);
    
    for (p2 -> puzzle[p2 -> s - 1] = (p2 -> s > 1 ? p2 -> puzzle[p2 -> s - 2] + 1 : 0); p2 -> puzzle[p2 -> s - 1] < p2 -> max_row; p2 -> puzzle[p2 -> s - 1]++){
      if (check(p2)){
	destroy_puzzle(p);
	p = p2;
	progress = true;
	break;
      }
    }
  }

  destroy_puzzle(p2);

  printf("Found (%d, %d) strong USP:\n", p -> s, k);
  print_puzzle(p);
  
  return 0;
}
