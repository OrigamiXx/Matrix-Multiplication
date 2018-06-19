#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <time.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "permutation.h"
#include "checker.h"
#include "heuristic.h"
#include "construct.h"

int main(int argc, char * argv[]){

  if (argc != 6 && argc != 3 && argc != 2 && argc != 1) {
    fprintf(stderr, "util_construct [<k> | <file1> <file2> | <s1> <k1> <s2> <k2> <iter>]\n");
    return -1;
  }

  int iter = 1000;
  
  if (argc == 1){

    // Mode 1: Randomly builds a table of strong USPs up to k = 6,
    // based on the twist constants and the sizes in ns.  Displays
    // puzzles which are "useful" to stdout with some statistics.
    // Uses randomized twisting.
    
    srand48(time(NULL));
    populate(iter, 6, true);


  } else if (argc == 2){

    // Mode 2: Randomly builds a table of strong USPs up to k =
    // atoi(argv[1]), based on the twist constants and the sizes in
    // ns.  Displays a graph (as adjency list CSV to stdout) of USPs
    // found for k-1, k-2, k-3 with edges between those that can be
    // (deterministically) twisted together.
    
    int k = atoi(argv[1]);
    assert(3 <= k && k <= 7);
    populate(iter, k-1, false);
    
    print_graph(k);
    
  } else if (argc == 3){

    // Mode 3: Takes two files containing USPs and attempts to twist
    // them together.
    
    puzzle * p1 = create_puzzle_from_file(argv[1]);
    if (p1 == NULL) {
      fprintf(stderr, "Error: Puzzle One does not exist or is incorrectly formated.\n");
      return -1;
    }
    
    puzzle * p2 = create_puzzle_from_file(argv[2]);
    if (p1 == NULL) {
      fprintf(stderr, "Error: Puzzle Two does not exist or is incorrectly formated.\n");
      return -1;
    }
    
    if (!check(p1)){
      fprintf(stderr, "Warning: Puzzle One is not a strong USP.\n");
    }
    
    if (!check(p2)){
      fprintf(stderr, "Warning: Puzzle Two is not a strong USP.\n");
    }
    
    // Make p1 the wider of the two.
    if (p1 -> k < p2 -> k){
      puzzle * tmp = p1;
      p1 = p2;
      p2 = tmp;
    }
    
    puzzle * p = create_full_twist(p1,p2);
    
    if (p){
      printf("Twisting successful:\n");
      print_puzzle(p);
      destroy_puzzle(p);
    } else {
      printf("No twisted puzzle found.\n");
    }
    
    destroy_puzzle(p1);
    destroy_puzzle(p2);
    
  } else if (argc == 6){

    // Mode 6: Attempts to deterministically twist randomly generated
    // USPs of the specified sizes.  Displays statistics.
    
    int s1 = atoi(argv[1]);
    int k1 = atoi(argv[2]);
    int s2 = atoi(argv[3]);
    int k2 = atoi(argv[4]);
    int iter = atoi(argv[5]);

    if (k1 < k2){
      int tmpk = k1;
      int tmps = s1;
      k1 = k2;
      s1 = s2;
      k2 = tmpk;
      s2 = tmps;
    }
    
    puzzle * p1 = create_puzzle(s1,k1);
    puzzle * p2 = create_puzzle(s2,k2);

    puzzle * p = create_puzzle(p1 -> s + p2 -> s, p1 -> k + 1);
    randomize_puzzle(p);
    
    srand48(time(NULL));
    
    int found = 0;

    for (int i = 0; i < iter; i++){

      random_usp(p1);
      random_usp(p2);

      puzzle * ret = create_full_twist(p1, p2);
      if (ret) {
	found++;
	destroy_puzzle(ret);
      }
      printf("Found = %d / %d\n", found, i+1);
      
    }

    printf("Found = %d / %d\n", found, iter);

    destroy_puzzle(p1);
    destroy_puzzle(p2);
    destroy_puzzle(p);
    
  }

  return 0;
 
}
