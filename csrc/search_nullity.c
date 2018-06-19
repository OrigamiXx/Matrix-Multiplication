#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "checker.h"
#include "puzzle.h"
#include "search_nullity.h"

int best_best = 0;

static int nullity_index_of_max(int to_check[], int total_elems) {

  //int total_elems = sizeof(*to_check) / sizeof(int);

  int curr_max = -1;
  int curr_max_index = -1;

  for (int i = 0; i < total_elems; i++) {
    if(curr_max_index == -1 || (to_check[i] != -1 && to_check[i] > to_check[curr_max_index])) {
      curr_max_index = i;
    }
  }
  return curr_max_index;
}

int nullity_search(puzzle * p, bool skip[], int skip_count, int best, int which, int heuristic_type) {
   
  bool local_skip[p->max_row];
  memcpy(local_skip, skip, sizeof(bool) * p->max_row);

  int s = p->s;
  int ret = s;

  // best = MAX(s,best);
  if (s > best) {
    best = s;
    // printf("at checkpoint one updated best %d and skipping %d on depth %d\n", best, skip_count, which);
  }

  if (best > best_best) {
    best_best = best;
    printf("a) BEST BEST UPDATED TO %d \n", best_best);
  }

  puzzle * new_puzzle = create_puzzle_from_puzzle(p, 0);

  // printf("best is %d and s is %d\n", best, new_puzzle->s);

  puzzle_row * U = new_puzzle->puzzle;

  int heuristic_results[new_puzzle->max_row];
  puzzle_row heuristic_rows[new_puzzle->max_row];

  for (U[s] = 0; U[s] < new_puzzle->max_row; U[s]++) {

    if ((U[s] < U[s-1] + 1 && s > 0) || local_skip[U[s]] || IS_USP != check(new_puzzle)) {
      heuristic_results[U[s]] = -1;
      heuristic_rows[U[s]] = U[s];

      if (!local_skip[U[s]]){
        local_skip[U[s]] = true;
        skip_count ++;
      }

    } else {
      // printf("pre heuristic\n");
      // heuristic_results[U[s]] = nullity_h(new_puzzle, local_skip, skip_count, which);
      heuristic_results[U[s]] = generic_h(new_puzzle, local_skip, skip_count, which, heuristic_type);
      // printf("post heuristic\n");
      heuristic_rows[U[s]] = U[s];
    }
  }


  int working_max_index = nullity_index_of_max(heuristic_results, new_puzzle->max_row);
  // printf("prewhile %d \n", heuristic_results[working_max_index]);


  // fix from using selection sort
  while (working_max_index != -1 && heuristic_results[working_max_index] > 0) {

    if (best >= heuristic_results[working_max_index] + p->s + 1){
      // printf("better than heuristic\n");
      return best;
    }

    // printf("current heuristic is %d\n", heuristic_results[working_max_index]);
    
    U[s] = heuristic_rows[working_max_index];


    int search_result = nullity_search(new_puzzle, local_skip, skip_count, best, which, heuristic_type);
    // int search_result = brute_force(new_puzzle, local_skip, skip_count, 6);

    // best = MAX(best, search_result);
    if (search_result > best) {
      best = search_result;
      // printf("at checkpoint two updated best %d and skipping %d on depth %d\n", best, skip_count, which);
    }


    if (best > best_best) {
      best_best = best;
      printf("b) BEST BEST UPDATED TO %d \n", best_best);
    }

    // drop the heuristic of that result so we don't try that again
    heuristic_results[working_max_index] = -1;
    working_max_index = nullity_index_of_max(heuristic_results, new_puzzle->max_row);
  }

  if (working_max_index != -1 && heuristic_results[working_max_index] == 0) {
    best = MAX(best, p->s+1);
  }
  
  if (best > best_best) {
    best_best = best;
    printf("c) BEST BEST UPDATED TO %d \n", best_best);
  }

  // printf("postwhile\n");

  destroy_puzzle(new_puzzle);


  return best;


}

int generic_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type) {
  switch (heuristic_type) {
    case 1:
      return nullity_h(p, skip, skip_count, which, heuristic_type);
    case 2:
      return clique_h(p, skip, skip_count, which, heuristic_type);
    default:
      fprintf(stderr, "Invalid heuristic type used\n");
      assert(false);
  }
}

int nullity_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type) {
  if (p->s < 8 || which >= 2){
    return 99999;
  }

  bool local_skip[p->max_row];
  memcpy(local_skip, skip, sizeof(bool) * p->max_row);
  
  int s = p->s;

  puzzle * new_puzzle = create_puzzle_from_puzzle(p, 0);
  puzzle_row * U = new_puzzle->puzzle;

  for (U[s] = 0; U[s] < new_puzzle->max_row; U[s]++) {
    if (local_skip[U[s]]) continue;

    if ((U[s] < U[s-1] + 1 && s > 0) || IS_USP != check(new_puzzle)) {
      local_skip[U[s]] = true;
      skip_count ++;
    }

  }

  destroy_puzzle(new_puzzle);
  // everything being skipped so no point continuing
  if (skip_count == new_puzzle->max_row){
    // printf("\rheuristic return 0");
    // fflush(stdout);
    return 0;
  }

  puzzle * blank_puzzle = create_puzzle(0, p->k);

  int bbb = nullity_search(blank_puzzle, local_skip, skip_count, 0, which+1, heuristic_type);
  // printf("\rsearched and heuristic return %d", bbb);
  // fflush(stdout);
  destroy_puzzle(blank_puzzle);
  return bbb;

}

int clique_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type) {
  return 0;
}

int brute_force(puzzle * p, bool skip[], int skipping, int up_to_size){

  bool local_skip[p->max_row];
  
  // make a local copy of the skip table so that we don't corrupt the previous one
  memcpy(local_skip, skip, sizeof(bool) * p->max_row);
  
  // the best by default is the size of the puzzle we start with
  int s = p->s;
  int ret = s;

  // make a new puzzle that is identical but a row longer
  puzzle * new_puzzle = create_puzzle_from_puzzle(p, 0);
  puzzle_row * U = new_puzzle -> puzzle;

  // the last row starts at value 0, goes until max row
  for (U[s] = 0; U[s] < new_puzzle->max_row && ret < up_to_size; U[s]++){
    
    // if we're skipping this row then skip it
    if (local_skip[U[s]]) continue;

    // else check if it's a usp
    if (IS_USP == check(new_puzzle)){

      // update the result and search further
      int res = brute_force(new_puzzle, local_skip, skipping, up_to_size);
      // choose between what we found and what we already had
      ret = (res > ret ? res : ret);

    } else {
      // if it wasn't a usp, skip it in the future loop iterations
      local_skip[U[s]] = true;
      skipping++;
    }
    
  }

  // do for loop
  // check for "best" one
  // break out early if best is gonna be worse than current best

  destroy_puzzle(new_puzzle);

  return ret;

}

int main(int argc, char ** argv) {

  if (argc != 4){
    fprintf(stderr, "usage: search_nullity <s> <k> <heuristic type>\n");
    fprintf(stderr, "Heuristic types:\n # 1 = nullity\n # 2 = clique\n");
    return -1;
  }

  int s = atoi(argv[1]);
  int k = atoi(argv[2]);
  int h_type = atoi(argv[3]);

  if (h_type < 1 || h_type > 2) {
    fprintf(stderr, "Invalid heuristic type used\n");
    return -1;
  }

  puzzle * start_p = create_puzzle(0, k);

  bool init_skip[start_p->max_row];
  bzero(init_skip, sizeof(init_skip));
  // int result = brute_force(start_p, init_skip, 0, s);

  int result = nullity_search(start_p, init_skip, 0, 0, 0, h_type);

  printf("Max rows found: %d\n", result);

}