#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "checker.h"
#include "puzzle.h"
#include "searcher.h"

int best_best = 0;

static int nullity_index_of_max(int to_check[], int total_elems) {

  int curr_max_index = -1;

  for (int i = 0; i < total_elems; i++) {
    if(curr_max_index == -1 || (to_check[i] != -1 && to_check[i] > to_check[curr_max_index])) {
      curr_max_index = i;
    }
  }
  return curr_max_index;
}

int search(puzzle * p, bool skip[], int skip_count, int best, int which, int heuristic_type) {
   
  bool local_skip[p->max_row];
  memcpy(local_skip, skip, sizeof(bool) * p->max_row);

  int s = p->s;

  best = MAX(s,best);

  if (best > best_best) {
    best_best = best;
    printf("a) BEST BEST UPDATED TO %d \n", best_best);
  }

  puzzle * new_puzzle = create_puzzle_from_puzzle(p, 0);
  puzzle_row * U = new_puzzle->puzzle;

  /* HEURISTIC GATHERING STARTS HERE */
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
      heuristic_results[U[s]] = generic_h(new_puzzle, local_skip, skip_count, which, heuristic_type);
      heuristic_rows[U[s]] = U[s];
    }
  }
  /* HEURISTIC GATHERING ENDS HERE */

  /* FURTHER SEARCHING BEGINS HERE */
  int working_max_index = nullity_index_of_max(heuristic_results, new_puzzle->max_row);

  // fix from using selection sort
  while (working_max_index != -1 && heuristic_results[working_max_index] >= 0) {
    
    if (best >= heuristic_results[working_max_index] + p->s + 1){
      return best;
    }
    
    U[s] = heuristic_rows[working_max_index];

    int search_result = search(new_puzzle, local_skip, skip_count, best, which, heuristic_type);

    best = MAX(best, search_result);

    if (best > best_best) {
      best_best = best;
      printf("b) BEST BEST UPDATED TO %d \n", best_best);
    }

    /*
    Drop the heuristic of that result so we don't try that again
    */
    heuristic_results[working_max_index] = -1;
    working_max_index = nullity_index_of_max(heuristic_results, new_puzzle->max_row);
  }
  /* FURTHER SEARCHING ENDS HERE */

  destroy_puzzle(new_puzzle);

  return best;
}

// int * calculate_heuristics(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type) {
//   switch (heuristic_type) {
//     case 0:
//     case 1:
//       // calculate the heuristic for each one
//       // return a list with all of them
//     case 2:
//       // call the heuristic function which inherently calculates all of them
//   }
// }

int generic_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type) {
  switch (heuristic_type) {
    case 0:
      return nullity_h(p, skip, skip_count, which, heuristic_type);
    case 1:
      //return clique_h(p, skip, skip_count, which, heuristic_type);
      return clique_h(p, 1);
    default:
      fprintf(stderr, "Invalid heuristic type used\n");
      assert(false);
  }
}

int nullity_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type) {
  // if (p->s < 8 || which >= 2){
  //   return 99999;
  // }

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

  int bbb = search(blank_puzzle, local_skip, skip_count, 0, which+1, heuristic_type);
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

/* 
 * Makes a 2d bool array with each layer size max_index
 * Each element is defaulted to false
 */
bool ** make_2d_bool_array(unsigned long max_index) {
  bool ** arr_2d = (bool **) malloc(sizeof(bool *) * max_index);
  bzero(arr_2d, sizeof(bool *) * max_index);
  for (unsigned long i = 0; i < max_index; i++) {
    arr_2d[i] = (bool *) malloc(sizeof(bool) * max_index);
    bzero(arr_2d[i], sizeof(bool) * max_index);
    for (unsigned long j = 0; j < max_index; j++) {
      arr_2d[i][j] = false;
    }
  }

  return arr_2d;
}

void copy_2d_bool_array_contents(bool ** dest, bool ** src, unsigned long max_index) {
  for (unsigned long i = 0; i < max_index; i++) {
    memcpy(dest[i], src[i], sizeof(bool) * max_index);
  }
}

/*
 * Frees each element in a 2d bool array
 * and then frees the array itself
 */
void free_2d_bool_array(bool ** arr_2d, unsigned long max_index) {
  for (unsigned long i = 0; i < max_index; i++) {
    free(arr_2d[i]);
  }
  free(arr_2d);
}

/*
 * Uses a puzzle, start u, max u, and max v
 * to generate the graph from adding different layers to a puzzle
 */
bool ** make_graph_from_puzzle(puzzle * p, puzzle_row start_u, puzzle_row max_u, puzzle_row max_v) {

  bool ** graph = make_2d_bool_array(MAX(max_u, max_v));
  int last_index = p->s - 1;
  
  for (puzzle_row u = start_u; u < max_u; u ++) {
    /*
    Start v at u+1 since it's an ordered puzzle
    */
    for (puzzle_row v = u+1; v < max_v; v ++) {
      
      p->puzzle[last_index-1] = u;
      p->puzzle[last_index] = v;
      /*
      Save a check => if u == v then it won't be a SUSP
      Shouldn't ever actually happen but... just in case
      */
      if (u == v) {
        graph[u][v] = false;
      } else if (graph[u][v] || graph[v][u]) {
          graph[u][v] = true;
          graph[v][u] = true;
      } else if (IS_USP == check(p)) {
        graph[u][v] = true;
        graph[v][u] = true;
      } else {
          /*
          Set the skip_list appropriately and also keep it mirrored
          */
          graph[u][v] = false;
          graph[v][u] = false;
      }
    }
  }

  return graph;
}

/*
 * Counts the connectedness of each vertex in a given graph
 * Returns a boolean representing whether or not any vertices at all exist
 */
bool count_vertices(unsigned long vertex_counter[], bool ** graph, puzzle_row max_graph_row) {
  bool node_exists = false;
  for (puzzle_row u = 0; u < max_graph_row; u++ ){
    vertex_counter[u] = 0;
    for (puzzle_row v = 0; v < max_graph_row; v++) {
      if (graph[u][v]) {
        vertex_counter[u] ++;
        node_exists = true;
      }
    }
  }
  return node_exists;

}

/*
 * Loop that goes each vertex
 * If one of the vertices is "less connected" than the clique we're testing for,
 * then remove that vertex from the graph and updates vertices appropriately
*/
bool remove_too_unconnected(unsigned long test_clique, unsigned long * vertices, bool ** graph, unsigned long max_graph_index) {
  bool changed = false;
  for (puzzle_row u = 0; u < max_graph_index; u ++) {
    if (vertices[u] < test_clique) {
      for (puzzle_row v = 0; v < max_graph_index; v ++) {
        if (graph[u][v]) {
          changed = true;
          vertices[u] --;
          vertices[v] --;
          graph[u][v] = false;
          graph[v][u] = false;
        }
      }
    }
  }

  return changed;
}

int clique_h(puzzle *p, int entries_per_vertex) {

  int k = p->k;
  puzzle_row max_index = ipow(ipow(3, k), entries_per_vertex);

  /* SET THE INITIAL BOUNDS TO SEARCH BETWEEN */
  long unsigned curr_lower_bound = 0;
  long unsigned curr_upper_bound = max_index;

  /* START SEARCHING ON ... */
  long unsigned current_test_clique = (curr_lower_bound + curr_upper_bound) / 2;

  /*
  Extend the puzzle by 2 so that we can play with the last 2 rows when testing
  */
  puzzle * test_puzzle = extend_puzzle(p, 2);
  int last_index = test_puzzle->s - 1;

  // Start at previous highest +1 since the puzzle is ordered
  puzzle_row next_start = test_puzzle->puzzle[last_index-1] + 1;

  bool ** graph = make_graph_from_puzzle(test_puzzle, next_start, max_index-1, max_index);
  
  unsigned long * good_vertices = (unsigned long *) malloc(sizeof(unsigned long *) * max_index);
  bzero(good_vertices, sizeof(unsigned long *) * max_index);

  bool node_exists = count_vertices(good_vertices, graph, max_index);

  unsigned long * test_vertices = (unsigned long *) malloc(sizeof(unsigned long *) * max_index);
  memcpy(test_vertices, good_vertices, sizeof(unsigned long *) * max_index);

  bool ** test_graph = make_2d_bool_array(max_index);

  bool found = false;
  while (!found) {

    copy_2d_bool_array_contents(test_graph, graph, max_index);

    // Reduce the graph to a fully connected graph
    bool changed = true;
    while (changed) {
      changed = remove_too_unconnected(current_test_clique, test_vertices, test_graph, max_index);
    }

    /*
    If the most connected vertex has 0 connections, the previous clique was the max clique
    */
    unsigned long max = test_vertices[0];
    for (puzzle_row i = 0; i < max_index; i ++) {
      if (test_vertices[i] > max) {
        max = test_vertices[i];
      }
    }

    if (max == 0) {
      curr_upper_bound = current_test_clique;
      memcpy(test_vertices, good_vertices, sizeof(unsigned long *) * max_index);
    } else {
      curr_lower_bound = current_test_clique;
      curr_upper_bound = MIN(curr_upper_bound, max);
      memcpy(test_vertices, good_vertices, sizeof(unsigned long *) * max_index);
    }

    if (curr_upper_bound - curr_lower_bound >= 0 && curr_upper_bound - curr_lower_bound <= 1) {
      found = true;
      if (curr_upper_bound == 0 && node_exists) {
        curr_upper_bound = 1;
      }
    } else {
      current_test_clique = (curr_upper_bound + curr_lower_bound) / 2;
    }
  }

  // Free memory from storing vertex information
  free(test_vertices);
  free(good_vertices);

  // Free memory from storing the graph
  free_2d_bool_array(test_graph, max_index);
  free_2d_bool_array(graph, max_index);

  destroy_puzzle(test_puzzle);
  // printf("Found the bound -- upper: %lu -- lower: %lu\n", curr_upper_bound, curr_lower_bound);
  return curr_upper_bound;

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