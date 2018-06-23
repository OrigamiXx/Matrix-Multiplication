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
      // heuristic_results[U[s]] = nullity_h(new_puzzle, local_skip, skip_count, which);
      heuristic_results[U[s]] = generic_h(new_puzzle, local_skip, skip_count, which, heuristic_type);
      heuristic_rows[U[s]] = U[s];
    }
  }


  int working_max_index = nullity_index_of_max(heuristic_results, new_puzzle->max_row);
  // printf("prewhile %d \n", heuristic_results[working_max_index]);


  // fix from using selection sort
  while (working_max_index != -1 && heuristic_results[working_max_index] >= 0) {
    
    if (best >= heuristic_results[working_max_index] + p->s + 1){
      // printf("better than heuristic\n");
      return best;
    }

    // printf("current heuristic is %d\n", heuristic_results[working_max_index]);
    
    U[s] = heuristic_rows[working_max_index];


    int search_result = nullity_search(new_puzzle, local_skip, skip_count, best, which, heuristic_type);

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

  // if (working_max_index != -1 && heuristic_results[working_max_index] == 0) {
  //   best += 1;
  //   if (best > best_best) {
  //     best_best = best;
  //     printf("fucking bested besty %d\n", best_best);
  //   }
  // }

  destroy_puzzle(new_puzzle);

  return best;
}

int generic_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type) {
  switch (heuristic_type) {
    case 1:
      return nullity_h(p, skip, skip_count, which, heuristic_type);
    case 2:
      //return clique_h(p, skip, skip_count, which, heuristic_type);
      return clique_h(p, 1, (bool*) NULL, (bool*) NULL);
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

static int ipow(int base, int exp) {
  int result = 1;
  while (exp) {
    if (exp & 1){
      result *= base;
    }
    exp >>=1;
    base *=base;
  }

  return result;
}

int clique_h(puzzle *p, int entries_per_vertex, bool * skip_list, bool * hit_list) {

  // do similar to skip list

  int current_max_clique = 0;

  int k = p->k;
  puzzle_row max_index = ipow(ipow(3, k), entries_per_vertex);

  // Extend the puzzle by 2 so that we can play with the last 2 rows when testing
  puzzle * temp_puzzle = create_puzzle_from_puzzle(p, 0);
  puzzle * test_puzzle = create_puzzle_from_puzzle(temp_puzzle, 0);
  int last_index = test_puzzle->s - 1;
  destroy_puzzle(temp_puzzle);

  // Start at previous highest +1 since the puzzle is ordered
  puzzle_row next_start = test_puzzle->puzzle[last_index-1] + 1;

  /*
  Equivalent to making bool graph[max_index][max_index], 
  and defaulting everything to false
  */
  bool ** graph = (bool **) malloc(sizeof(bool *) * max_index);
  bzero(graph, sizeof(graph));
  for (int i = 0; i < max_index; i++) {
    graph[i] = (bool *) malloc(sizeof(bool) * max_index);
    bzero(graph[i], sizeof(graph[i]));
    for (int j = 0; j < max_index; j++) {
      graph[i][j] = false;
    }
  }

  /*
  Start u at next_start so we don't waste time - the puzzle is ordered
  */
  for (puzzle_row u = next_start; u < max_index; u ++) {
    /*
    Start v at u+1 since it's an ordered USP
    */
    for (puzzle_row v = u+1; v < max_index; v ++) {

      /*
      Save a check => if u == v then it won't be a SUSP
      Shouldn't ever actually happen but... just in case
      */
      if (u == v) {
        graph[u][v] = false;
      } else {
        test_puzzle->puzzle[last_index-1] = u;
        test_puzzle->puzzle[last_index] = v;
        
        // Don't bother checking if we've got results there
        if (graph[u][v] || graph[v][u]) {
          /* If at least one is there, make sure it's mirrored */
          graph[u][v] = true;
          graph[v][u] = true;
        // } else if (skip_list[u] || skip_list[v]) {
          // printf("Skipped thanks to the skip list!")
          // graph[u][v] = false;
          // graph[v][u] = false;
        } else if (IS_USP == check(test_puzzle)) { 
          graph[u][v] = true;
          graph[v][u] = true; /* Mirror to optimize */
        } else {
          graph[u][v] = false;
          graph[v][u] = false;

          /*
          Set the skip_list appropriately and also keep it mirrored
          */
          // skip_list[u] = true;
          // skip_list[v] = true;

        }
      }
    }
  }
  
  unsigned long * vertices = (unsigned long *) malloc(sizeof(unsigned long *) * max_index);
  bzero(vertices, sizeof(vertices));
  
  bool found = false;
  while (!found) {
    /*
    Count the connectness for each vertex
    */
    for (puzzle_row u = 0; u < max_index; u++ ){
      vertices[u] = 0;
      for (puzzle_row v = 0; u < max_index; u++) {
        if (graph[u][v]) {
          vertices[u] ++;
        }
      }
    }

    /*
    A loop to reduce the graph down to a fully connected graph
    */
    bool changed = true;
    while (changed) {
      changed = false;

      /*
      Loop that goes through each set of vertices
      If one set of vertices is "less connected" than the current max clique requires,
      then remove that vertex from the graph
      */
      for (puzzle_row u = 0; u < max_index; u ++) {
        if (vertices[u] < current_max_clique) {
          // printf("dafuq %lu\n", vertices[u]);
          for (puzzle_row v = 0; v < max_index; v ++) {
            if (graph[u][v]) {
              changed = true;
              graph[u][v] = false;
              graph[v][u] = false;
            }
          }
        }
      }

      /*
      Re-count the connectedness for each vertex
      */
      for (puzzle_row u = 0; u < max_index; u ++) {
        vertices[u] = 0;

        for (puzzle_row v = 0; v < max_index; v ++) {
          if (graph[u][v]) {
            vertices[u] ++;
          }
        }
      }
    }

    /*
    If the most connected vertex has 0 connections, the previous clique was the max clique
    */
    unsigned long max = vertices[0];
    unsigned long nonzero_min = vertices[0];
    for (puzzle_row i = 0; i < max_index; i ++) {
      if (vertices[i] > max) {
        max = vertices[i];
      }

      if (nonzero_min == 0 || (vertices[i] < nonzero_min && vertices[i] > 0)) {
        nonzero_min = vertices[i];
      }
    }

    if (max == 0) {
      found = true;
    } else {
      // Easier and better (?) than binary search
      current_max_clique = MAX(current_max_clique + 1, nonzero_min+1); 
      if (current_max_clique > 100) {
        current_max_clique = 100;
        found = true;
      }
    }
  }

  // Free memory from storing vertex information
  free(vertices);
  
  // Free memory from storing the graph
  for (int i = 0; i < max_index; i++) {
    free(graph[i]);
  }
  free(graph);

  destroy_puzzle(test_puzzle);
  printf("found a max clique %d\n", current_max_clique);
  return current_max_clique;

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