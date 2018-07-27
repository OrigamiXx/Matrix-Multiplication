#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <queue>
#include <string>
#include <time.h>

#include "constants.h"
#include "checker.h"
#include "puzzle.h"
#include "searcher.h"
#include "timing.h"
#include "clique_to_mip.h"
#include "canonization.h"

clockid_t clock_mode = CLOCK_MONOTONIC;

std::priority_queue<level_heuristic> heuristics_at_level;

// proper mental, innit?
int number_of_heuristics = 5;
const char * heuristic_names[] = {
  "nullity",
  "clique_approximation",
  "inline_vertex_degree",
  "inline_clique_approximation",
  "clique_mip"
};

int best_best = 0;
cumulative_tracker * heuristic_details = NULL;

struct timespec last_log_time;
char * log_file_name;

// char * heuristic_int_to_name(int h_type) {
//   return 
// }

void init_log(char * given_name) {
  log_file_name = given_name;
  struct timespec init_log_time = {0,0};
  clock_gettime(clock_mode, &init_log_time);
  last_log_time = init_log_time;

  FILE * log_file;
  log_file = fopen(log_file_name, "w");

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  fprintf(log_file, "Log started at time %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  fprintf(log_file, "heuristic_type,level,graph_time,total_time,num_heuristics_gathered,total_heuristic_depth,average_heuristic_depth,max_heuristic_depth\n");
  fclose(log_file);


}

void log_current_results(bool force_log) {

  double logging_interval_seconds = 60 * 3;
  
  if (heuristic_details != NULL) {
    
    struct timespec current_log_time = {0,0};
    clock_gettime(clock_mode, &current_log_time);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    double time_since_last_log = (((double)current_log_time.tv_sec + 1.0e-9*current_log_time.tv_nsec) - ((double)last_log_time.tv_sec + 1.0e-9*last_log_time.tv_nsec));

    if (force_log || time_since_last_log > logging_interval_seconds) {
      
      FILE * log_file;
      log_file = fopen(log_file_name, "a");

      fprintf(log_file, "Logging details at time %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
      fprintf(log_file, "Time since last log: %e ; logging interval: %e\n", time_since_last_log, logging_interval_seconds);
      fprintf(log_file, "heuristics: ");
      for (int i = 0; i < number_of_heuristics; i++) {
        fprintf(log_file, " # %d = %s ;", i, heuristic_names[i]);
      }
      fprintf(log_file, "\n");

      fprintf(log_file, "heuristic_type,level,graph_time,total_time,num_heuristics_gathered,total_heuristic_depth,average_heuristic_depth,max_heuristic_depth\n");
      for (int h_type = 0; h_type < number_of_heuristics; h_type++) {
        for (int level = 0; level < heuristic_details[h_type].num_details; level++) {

          double g_time = heuristic_details[h_type].details[level].graph_time_at_level;
          double t_time = heuristic_details[h_type].details[level].total_time_at_level;
          int num_h = heuristic_details[h_type].details[level].num_heuristics;
          double ave_d = ((double) (heuristic_details[h_type].details[level].total_depth)) / ((double) (MAX(1, heuristic_details[h_type].details[level].num_heuristics)));
          // double max_d = (num_h == 0 ? level : heuristic_details[h_type].details[level].max_depth);
          double max_d = heuristic_details[h_type].details[level].max_depth;
          double tot_d = heuristic_details[h_type].details[level].total_depth;

          //              | graph_time | total_time | heuristics_gathered | average_depth | max_depth | min_depth
          // htype, level | 
          fprintf(log_file, "%d,%d,%e,%e,%d,%e,%e, %e\n", h_type, level, g_time, t_time, num_h, tot_d, ave_d, max_d);

        }
      }

      fprintf(log_file, "-------- Logging iteration complete --------\n");
      fclose(log_file);

      last_log_time = current_log_time;
      //              | graph_time | total_time | heuristics_gathered | average_depth | max_depth | min_depth
      // htype, level | 

      return;
    }
  }
}

void move_results_global(std::priority_queue<heuristic_result> heuristic_queue, puzzle * puzzle) {

  while (!heuristic_queue.empty()) {
    heuristic_result current_heur = heuristic_queue.top();
    heuristic_queue.pop();

    level_heuristic global_heur;
    global_heur.total_heuristic = puzzle->s + current_heur.result;
    global_heur.level_puzzle = create_puzzle_copy(puzzle);

    heuristics_at_level.push(global_heur);
  }

}

void wipe_statistics() {

  for (int h_type = 0; h_type < number_of_heuristics; h_type ++) {
    free(heuristic_details[h_type].details);
  }

  free(heuristic_details);
  heuristic_details = NULL;
}

void check_extend_heuristic_details(int heuristic_type, int current_level) {

  if (heuristic_details == NULL) {
    heuristic_details = (cumulative_tracker *) malloc(sizeof(cumulative_tracker) * number_of_heuristics);

    bzero(heuristic_details, sizeof(cumulative_tracker) * number_of_heuristics);
    for (int i = 0; i < number_of_heuristics; i++) {
      heuristic_details[i].num_details = 0;
      heuristic_details[i].details = NULL;
    }


  }
  if (heuristic_details[heuristic_type].details == NULL || (current_level+1) > heuristic_details[heuristic_type].num_details) {

    cumulative_counts * new_heuristic_details = (cumulative_counts *) malloc(sizeof(cumulative_counts) * (current_level+1));
    bzero(new_heuristic_details, sizeof(cumulative_counts) * (current_level+1));

    int previous_num_details;
    if (heuristic_details == NULL) {
      previous_num_details = 0;
    } else {
      previous_num_details = heuristic_details[heuristic_type].num_details;
      memcpy(new_heuristic_details, heuristic_details[heuristic_type].details, sizeof(cumulative_counts) * previous_num_details);

    }


    for (int i = previous_num_details; i < (current_level+1); i ++) {
      new_heuristic_details[i].start_time = {0,0};
      new_heuristic_details[i].is_timing = false;

      new_heuristic_details[i].graph_start_time = {0,0};
      new_heuristic_details[i].graph_is_timing = false;

      new_heuristic_details[i].total_time_at_level = 0;
      new_heuristic_details[i].total_instances_at_level = 0;
      new_heuristic_details[i].num_heuristics = 0;
      new_heuristic_details[i].total_heuristic = 0;
    }

    free(heuristic_details[heuristic_type].details);
    heuristic_details[heuristic_type].details = new_heuristic_details;
    heuristic_details[heuristic_type].num_details = (current_level+1);

  }
}

void add_level_instance(int heuristic_type, int current_level) {
  heuristic_details[heuristic_type].details[current_level].total_instances_at_level ++;
}

void start_timer(int heuristic_type, int current_level) {

  assert(current_level < heuristic_details[heuristic_type].num_details);
  assert(!heuristic_details[heuristic_type].details[current_level].is_timing);

  if(!heuristic_details[heuristic_type].details[current_level].is_timing) {

    heuristic_details[heuristic_type].details[current_level].is_timing = true;
    heuristic_details[heuristic_type].details[current_level].start_time = {0,0};

    clock_gettime(clock_mode, &(heuristic_details[heuristic_type].details[current_level].start_time));
  }


}

void stop_timer(int heuristic_type, int current_level) {

  assert(current_level < heuristic_details[heuristic_type].num_details);
  assert(heuristic_details[heuristic_type].details[current_level].is_timing);

  struct timespec end = {0,0};

  clock_gettime(clock_mode, &end);

  double time_difference = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)heuristic_details[heuristic_type].details[current_level].start_time.tv_sec + 1.0e-9*heuristic_details[heuristic_type].details[current_level].start_time.tv_nsec);

  heuristic_details[heuristic_type].details[current_level].total_time_at_level += time_difference;
  heuristic_details[heuristic_type].details[current_level].is_timing = false;
}

void start_graph_timer(int heuristic_type, int current_level) {
  assert(current_level < heuristic_details[heuristic_type].num_details);
  assert(!heuristic_details[heuristic_type].details[current_level].graph_is_timing);

  heuristic_details[heuristic_type].details[current_level].graph_is_timing = true;
  heuristic_details[heuristic_type].details[current_level].graph_start_time = {0,0};

  clock_gettime(clock_mode, &(heuristic_details[heuristic_type].details[current_level].graph_start_time));
}

void stop_graph_timer(int heuristic_type, int current_level) {
  assert(current_level < heuristic_details[heuristic_type].num_details);
  assert(heuristic_details[heuristic_type].details[current_level].graph_is_timing);

  struct timespec end = {0,0};

  clock_gettime(clock_mode, &end);

  double time_difference = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)heuristic_details[heuristic_type].details[current_level].graph_start_time.tv_sec + 1.0e-9*heuristic_details[heuristic_type].details[current_level].graph_start_time.tv_nsec);

  heuristic_details[heuristic_type].details[current_level].graph_time_at_level += time_difference;
  heuristic_details[heuristic_type].details[current_level].graph_is_timing = false;
}

// Assumes puzzle already has extra row to play with
void calculate_heuristics(puzzle * p, std::priority_queue<heuristic_result> * h_queue, bool skip[], int * skip_count, int which, int heuristic_type, int stop_at_s, int heuristic_stage) {
  int heuristic_results[p->max_row];
  bzero(heuristic_results, sizeof(heuristic_results));

  puzzle_row * U = p->puzzle;
  int s = p->s - 1;

  for (U[s] = 0; U[s] < p->max_row; U[s]++) {

    if ((U[s] < U[s-1] + 1 && s > 0) || skip[U[s]] || IS_USP != check(p) || !have_seen_isomorph(p)) {
      heuristic_results[U[s]] = -1;

      if (!skip[U[s]]){
        skip[U[s]] = true;
        (*skip_count) ++;
      }

    }
  }

  for (U[s] = 0; U[s] < p->max_row; U[s]++) {
    if (!skip[U[s]]) {
      // printf("\r%f", ((double) U[s] / (double) p->max_row) * 100);
      // fflush(stdout);
      heuristic_results[U[s]] = generic_h(p, skip, *skip_count, which, heuristic_type, stop_at_s, heuristic_stage);
    }
  }
  // printf("\r");
  // fflush(stdout);

  long total_heur = 0;
  long num_ignored = 0;
  long num_to_check = 0;
  for (puzzle_row u = 0; u < p->max_row; u++) {
    if (heuristic_results[u] == -1) {
      num_ignored += 1;
    } else {
      num_to_check += 1;
      total_heur += heuristic_results[u];

      heuristic_details[heuristic_type].details[which].num_heuristics ++;
      heuristic_details[heuristic_type].details[which].total_heuristic += heuristic_results[u];

      heuristic_details[heuristic_type].details[which].max_heuristic = MAX(heuristic_results[u], heuristic_details[heuristic_type].details[which].max_heuristic);
      heuristic_details[heuristic_type].details[which].min_heuristic = (heuristic_details[heuristic_type].details[which].min_heuristic == 0 ? heuristic_results[u] : MIN(heuristic_details[heuristic_type].details[which].min_heuristic, heuristic_results[u]));

      double depth_with_heuristic = p->s + heuristic_results[u];
      // printf("DEPTH VS WHICH IS %d vs %d\n", p->s-1, which);
      heuristic_details[heuristic_type].details[which].total_depth += depth_with_heuristic;
      heuristic_details[heuristic_type].details[which].max_depth = MAX(heuristic_details[heuristic_type].details[which].max_depth, depth_with_heuristic);
      heuristic_details[heuristic_type].details[which].min_depth = (heuristic_details[heuristic_type].details[which].min_depth == 0 ? depth_with_heuristic : MIN(heuristic_details[heuristic_type].details[which].min_depth, depth_with_heuristic));

      // printf("Depth with heuristic found is %e\n", depth_with_heuristic);
      // printf("Max depth is now %e\n", curr_details.max_depth);
      // printf("Double check %e\n", heuristic_details[heuristic_type].details[which].max_depth);


      struct heuristic_result this_result;
      this_result.value = u;
      this_result.result = heuristic_results[u];

      (*h_queue).push(this_result);
    }
  }

  if (num_to_check == 0) {
    // If there's nothing to explore, the max depth is still at least the puzzle without the new row.
    heuristic_details[heuristic_type].details[which].max_depth = MAX(p->s-1, heuristic_details[heuristic_type].details[which].max_depth);
  }

}

void extend_graph_from_puzzle(bool ** graph, puzzle * p, bool skip[], puzzle_row start_u, puzzle_row max_u, puzzle_row max_v, int which, int heuristic_type) {

  static bool seen = false;
  if(seen) {
    printf("seen\n");
    return;
    assert(0);
  } else {
    printf("not seen\n");
  }
  if(which >= 0) {
    start_graph_timer(heuristic_type, which);
  }

  puzzle * testing_puzzle = extend_puzzle(p, 2);
  int last_index = testing_puzzle->s - 1;

  for (puzzle_row u = start_u; u < max_u; u++) {
    for (puzzle_row v = u+1; v < max_v; v++) {
      seen = true;
      
      testing_puzzle->puzzle[last_index-1] = u;
      testing_puzzle->puzzle[last_index] = v;

      if (u == v) {
        graph[u][v] = false;
        graph[v][u] = false;
      } else if (skip[u] || skip[v]) {
        graph[u][v] = false;
        graph[v][u] = false;
      } else if (graph[u][v] || graph[v][u]) {
        graph[u][v] = true;
        graph[v][u] = true;
      } else if (IS_USP == check(testing_puzzle)) {
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

  destroy_puzzle(testing_puzzle);

  if (which >= 0) {
    stop_graph_timer(heuristic_type, which);
  }

}

int inline_search(puzzle * p, bool ** graph, bool skip[], int skip_count, int best, int which, int heuristic_type, int stop_at_s, int heuristic_stage) {
  log_current_results(false);
  
  check_extend_heuristic_details(heuristic_type, which);
  add_level_instance(heuristic_type, which);
  start_timer(heuristic_type, which);

  bool local_skip[p->max_row];
  memcpy(local_skip, skip, sizeof(bool) * p->max_row);
  int local_skip_count = skip_count;

  int s = p->s;

  best = MAX(s,best);

  if (best > best_best) {
    best_best = best;
    // printf("a) BEST BEST UPDATED TO %d %d\n", best_best, which);
  }

  puzzle * new_puzzle = extend_puzzle(p, 1);
  puzzle_row * U = new_puzzle->puzzle;

  int old_k = p->k;
  puzzle_row old_max_index = ipow2(3, old_k);

  int new_k = new_puzzle->k;
  puzzle_row new_max_index = ipow2(3, new_k);

  puzzle_row next_start = p->puzzle[p->s-1] + 1;

  bool ** local_graph = make_2d_bool_array(new_max_index);
  copy_2d_bool_array_contents(local_graph, graph, old_max_index);

  extend_graph_from_puzzle(local_graph, p, local_skip, next_start, new_max_index-1, new_max_index, which, heuristic_type);

  std::priority_queue<heuristic_result> heuristic_queue;
  printf("-----------------------\n");
  printf("About to calculate heuristics for puzzle\n");
  printf("Current heuristic is %s\n", heuristic_names[heuristic_type]);
  print_puzzle(p);
  printf("-----------------------\n");
  inline_h(new_puzzle, local_graph, &heuristic_queue, local_skip, local_skip_count, which, heuristic_type);

  if (best >= stop_at_s && heuristic_stage == 1) {
    move_results_global(heuristic_queue, new_puzzle);
  } else {
    /* FURTHER SEARCHING BEGINS HERE */
    while (!heuristic_queue.empty() && best < stop_at_s) {

      heuristic_result current_heur = heuristic_queue.top();
      heuristic_queue.pop();

      if (best >= current_heur.result + p->s + 1) {
        stop_timer(heuristic_type, which);
        return best;
      }

      U[s] = current_heur.value;

      //TOPRINT puzzle and heuristic that we're searching it for\

      printf("-----------------------\n");
      printf("Searching on puzzle with heuristic value %f\n", current_heur.result);
      printf("Current heuristic is %s\n", heuristic_names[heuristic_type]);
      print_puzzle(new_puzzle);
      printf("-----------------------\n");

      stop_timer(heuristic_type, which);
      int search_result = inline_search(new_puzzle, local_graph, local_skip, skip_count, best, which+1, heuristic_type, stop_at_s, heuristic_stage);
      start_timer(heuristic_type, which);

      best = MAX(best, search_result);
      if (best > best_best) {
        best_best = best;
        // printf("b) BEST BEST UPDATED TO %d \n", best_best);
      }

    }
  }

  

  destroy_puzzle(new_puzzle);
  free_2d_bool_array(local_graph, new_max_index);

  stop_timer(heuristic_type, which);
  return best;
}

int search(puzzle * p, bool skip[], int skip_count, int best, int which, int heuristic_type, int stop_at_s, bool stats_enabled, int heuristic_stage) {
  log_current_results(false);
  check_extend_heuristic_details(heuristic_type, which);
  if (stats_enabled) {
    add_level_instance(heuristic_type, which);
    start_timer(heuristic_type, which);
  }

  bool local_skip[p->max_row];
  memcpy(local_skip, skip, sizeof(bool) * p->max_row);
  int local_skip_count = skip_count;

  int s = p->s;

  best = MAX(s,best);

  if (best > best_best) {
    best_best = best;
    // printf("a) BEST BEST UPDATED TO %d \n", best_best);
  }

  puzzle * new_puzzle = extend_puzzle(p, 1);
  puzzle_row * U = new_puzzle->puzzle;

  /* FURTHER SEARCHING BEGINS HERE */

  // printf("level is %d ; best is %d ; stop is %d ; p->s is %d\n", which, best, stop_at_s, p->s);

  std::priority_queue<heuristic_result> heuristic_queue;
  printf("-----------------------\n");
  printf("About to calculate heuristics for puzzle\n");
  printf("Current heuristic is %s\n", heuristic_names[heuristic_type]);
  print_puzzle(p);
  printf("-----------------------\n");
  calculate_heuristics(new_puzzle, &heuristic_queue, local_skip, &local_skip_count, which, heuristic_type, stop_at_s, heuristic_stage);

  if (best >= stop_at_s && heuristic_stage == 1) {
    move_results_global(heuristic_queue, new_puzzle);
  } else {
    while(!heuristic_queue.empty() && best < stop_at_s) {
      heuristic_result current_heur = heuristic_queue.top();
      heuristic_queue.pop();
      if (best >= current_heur.result + p->s + 1) {
        if (stats_enabled) {
          stop_timer(heuristic_type, which);
        }
        return best;
      }

      U[s] = current_heur.value;

      //TOPRINT puzzle and heuristic that we're searching it for

      printf("-----------------------\n");
      printf("Searching on puzzle with heuristic value %f\n", current_heur.result);
      printf("Current heuristic is %s\n", heuristic_names[heuristic_type]);
      print_puzzle(new_puzzle);
      printf("-----------------------\n");
      

      if (stats_enabled) {
        stop_timer(heuristic_type, which);
      }

      int search_result = search(new_puzzle, local_skip, skip_count, best, which+1, heuristic_type, stop_at_s, true, heuristic_stage);

      if (stats_enabled) {
        start_timer(heuristic_type, which);
      }

      best = MAX(best, search_result);
      if (best > best_best) {
        // printf("b) BEST BEST UPDATED TO %d \n", best_best);
      }

    }
  }


  destroy_puzzle(new_puzzle);

  if (stats_enabled) {
    stop_timer(heuristic_type, which);
  }
  return best;
}

bool inline_remove_too_unconnected(puzzle_row test_clique, puzzle_row * vertices, bool ** graph, puzzle_row max_graph_index, int which, int * heuristic_results) {
  bool changed = false;
  for (puzzle_row u = 0; u < max_graph_index; u ++) {
    if (vertices[u] < test_clique) {
      if (heuristic_results[u] != 0 && heuristic_results[u] != -1) {
        heuristic_results[u] = MAX(heuristic_results[u], vertices[u]);
        // printf("changed value \n");
      } else if (heuristic_results[u] != -1){
        // printf("SET INTIIAL VALUE\n");
        heuristic_results[u] = MAX(vertices[u], 1);
      }

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

void inline_clique_h(puzzle * p, bool ** graph, puzzle_row * start_vertices, bool skip[], int which, int * heuristic_results){
  puzzle_row max_index = ipow2(3, p->k);

  puzzle_row curr_lower_bound = 0;
  puzzle_row curr_upper_bound = max_index;

  puzzle_row current_test_clique = (curr_lower_bound + curr_upper_bound) / 2;

  puzzle_row * test_vertices = (puzzle_row *) malloc(sizeof(puzzle_row) * max_index);
  bzero(test_vertices, sizeof(puzzle_row) * max_index);

  bool ** test_graph = make_2d_bool_array(max_index);

  bool found = false;
  while (!found) {
    copy_2d_bool_array_contents(test_graph, graph, max_index);

    bool changed = true;
    while (changed) {
      changed = inline_remove_too_unconnected(current_test_clique, test_vertices, test_graph, max_index, which, heuristic_results);
    }

    puzzle_row max = test_vertices[0];
    for (puzzle_row i = 0; i < max_index; i ++) {
      if (test_vertices[i] > max) {
        max = test_vertices[i];
      }
    }

    if (max == 0) {
      curr_upper_bound = current_test_clique;
      memcpy(test_vertices, start_vertices, sizeof(puzzle_row) * max_index);
    } else {
      curr_lower_bound = current_test_clique;
      curr_upper_bound = MIN(curr_upper_bound, max);
      memcpy(test_vertices, start_vertices, sizeof(puzzle_row) * max_index);
    }

    if (curr_upper_bound - curr_lower_bound >= 0 && curr_upper_bound - curr_lower_bound <= 1) {
      found = true;
      memcpy(start_vertices, test_vertices, sizeof(puzzle_row) * max_index);
    } else {
      current_test_clique = (curr_upper_bound + curr_lower_bound) / 2;
    }


  }

  free(test_vertices);
  free_2d_bool_array(test_graph, max_index);
}

int mip_clique_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type) {
  puzzle_row max_index = ipow2(3, p->k);
  puzzle_row next_start = p->puzzle[p->s-1] + 1;


  // printf("--------\n");
  // print_puzzle(p);
  // printf("--------\n");

  int graph_size;
  bool ** graph = make_graph_from_puzzle(p, skip, next_start, max_index, which, heuristic_type);
  
  int current_max_clique = (max_index > 0 ? max_clique_mip(graph, max_index) : 0);

  free_2d_bool_array(graph, max_index);

  return current_max_clique;

}

void inline_h(puzzle * p, bool ** graph, std::priority_queue<heuristic_result> * h_queue, bool skip[], int skip_count, int which, int heuristic_type) {

  int heuristic_results[p->max_row];
  bzero(heuristic_results, sizeof(heuristic_results));

  puzzle_row max_index = ipow2(3, p->k);
  puzzle_row * the_vertices = (puzzle_row *) malloc(sizeof(puzzle_row) * max_index);
  bzero(the_vertices, sizeof(puzzle_row) * max_index);

  bool node_exists = count_vertices(the_vertices, graph, max_index);

  puzzle_row * U = p->puzzle;
  int s = p->s - 1;

  for (U[s] = 0; U[s] < p->max_row; U[s]++) {
    if ((U[s] < U[s-1] + 1 && s > 0) || skip[U[s]] || IS_USP != check(p) || !have_seen_isomorph(p)) {
      heuristic_results[U[s]] = -1;

      if(!skip[U[s]]) {
        skip[U[s]] = true;
        skip_count ++;
      }
    }
  }

  switch(heuristic_type) {

    case 2:
      for (puzzle_row u = 0; u < max_index; u++) {
        if (!skip[u]) {
          heuristic_results[u] = the_vertices[u];
        }
      }
      break;
    case 3:
      inline_clique_h(p, graph, the_vertices, skip, which, heuristic_results);

      break;
    default:
      fprintf(stderr, "Invalid heuristic type used\n");
      assert(false);
  }

  free(the_vertices);

  long total_heur = 0;
  long num_ignored = 0;
  long num_to_check = 0;
  for (puzzle_row u = 0; u < max_index; u++) {
    if (heuristic_results[u] == -1) {
      num_ignored += 1;
    } else {
      num_to_check += 1;
      total_heur += heuristic_results[u];

      // heuristic_details[heuristic_type].details[which].num_heuristics ++;
      // heuristic_details[heuristic_type].details[which].total_heuristic += heuristic_results[u];

      // heuristic_details[heuristic_type].details[which].max_heuristic = MAX(heuristic_results[u], heuristic_details[heuristic_type].details[which].max_heuristic);
      // heuristic_details[heuristic_type].details[which].min_heuristic = (heuristic_details[heuristic_type].details[which].min_heuristic == 0 ? heuristic_results[u] : MIN(heuristic_results[u], heuristic_details[heuristic_type].details[which].min_heuristic)); 

      heuristic_details[heuristic_type].details[which].num_heuristics ++;
      heuristic_details[heuristic_type].details[which].total_heuristic += heuristic_results[u];

      heuristic_details[heuristic_type].details[which].max_heuristic = MAX(heuristic_results[u], heuristic_details[heuristic_type].details[which].max_heuristic);
      heuristic_details[heuristic_type].details[which].min_heuristic = (heuristic_details[heuristic_type].details[which].min_heuristic == 0 ? heuristic_results[u] : MIN(heuristic_details[heuristic_type].details[which].min_heuristic, heuristic_results[u]));

      double depth_with_heuristic = p->s + heuristic_results[u];
      heuristic_details[heuristic_type].details[which].total_depth += depth_with_heuristic;
      heuristic_details[heuristic_type].details[which].max_depth = MAX(heuristic_details[heuristic_type].details[which].max_depth, depth_with_heuristic);
      heuristic_details[heuristic_type].details[which].min_depth = (heuristic_details[heuristic_type].details[which].min_depth == 0 ? depth_with_heuristic : MIN(heuristic_details[heuristic_type].details[which].min_depth, depth_with_heuristic));

      // if (heuristic_details[heuristic_type].details[which].max_depth > heuristic_details[heuristic_type].details[which-1].max_depth) {
      //   U[s] = 
      //   assert(false);
      // }

      struct heuristic_result this_result;
      this_result.value = u;
      this_result.result = heuristic_results[u];

      (*h_queue).push(this_result);
    }
  }

  if (num_to_check == 0) {
    // If there's nothing to explore, the max depth is still at least the puzzle without the new row.
    heuristic_details[heuristic_type].details[which].max_depth = MAX(p->s-1, heuristic_details[heuristic_type].details[which].max_depth);
  }

}

int generic_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type, int stop_at_s, int heuristic_stage) {
  switch (heuristic_type) {
    case 0:
      return nullity_h(p, skip, skip_count, which, heuristic_type, stop_at_s, heuristic_stage);
    case 1:
      return clique_h(p, skip, skip_count, which, heuristic_type);
    case 4:
      return mip_clique_h(p, skip, skip_count, which, heuristic_type);
    default:
      fprintf(stderr, "Invalid heuristic type used\n");
      assert(false);
  }
}

int nullity_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type, int stop_at_s, int heuristic_stage) {

  bool local_skip[p->max_row];
  memcpy(local_skip, skip, sizeof(bool) * p->max_row);

  int s = p->s;

  puzzle * new_puzzle = extend_puzzle(p, 1);
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
    return 0;
  }

  puzzle * blank_puzzle = create_puzzle(0, p->k);

  int bbb = search(blank_puzzle, local_skip, skip_count, 0, which+1, heuristic_type, stop_at_s, false, heuristic_stage);
  // int bbb = search(blank_puzzle, local_skip, skip_count, 0, -1, heuristic_type, stop_at_s);
  destroy_puzzle(blank_puzzle);
  return bbb;

}

int ipow2(int base, int exp) {
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
bool ** make_2d_bool_array(puzzle_row max_index) {
  bool ** arr_2d = (bool **) malloc(sizeof(bool *) * max_index);
  bzero(arr_2d, sizeof(bool *) * max_index);
  for (puzzle_row i = 0; i < max_index; i++) {
    arr_2d[i] = (bool *) malloc(sizeof(bool) * max_index);
    bzero(arr_2d[i], sizeof(bool) * max_index);
    for (puzzle_row j = 0; j < max_index; j++) {
      arr_2d[i][j] = false;
    }
  }

  return arr_2d;
}

void copy_2d_bool_array_contents(bool ** dest, bool ** src, puzzle_row max_index) {
  for (puzzle_row i = 0; i < max_index; i++) {
    memcpy(dest[i], src[i], sizeof(bool) * max_index);
  }
}

/*
 * Frees each element in a 2d bool array
 * and then frees the array itself
 */
void free_2d_bool_array(bool ** arr_2d, puzzle_row max_index) {
  for (puzzle_row i = 0; i < max_index; i++) {
    free(arr_2d[i]);
  }
  free(arr_2d);
}

/*
 * Creates a puzzle with two more rows than the given puzzle p
 * Tests on the next final two rows to make a graph of edges between u and v
 * The first row is tested between start_u and max_u, the second row is tested between (first_row + 1) and max_v
 */
bool ** make_graph_from_puzzle(puzzle * p, bool skip[], puzzle_row start_u, puzzle_row &max_index, int which, int heuristic_type) {

  if (which >= 0) {
    start_graph_timer(heuristic_type, which);
  }

  bool local_skip[max_index];
  memcpy(local_skip, skip, sizeof(bool) * max_index);

  puzzle * testing_puzzle = extend_puzzle(p, 2);
  puzzle * inner_testing_puzzle = extend_puzzle(p, 1);

  int last_index = testing_puzzle->s - 1;

  int not_skipped = 0;
  
  for (puzzle_row u = start_u; u < max_index; u ++) {
    
    if (local_skip[u]) continue;

    inner_testing_puzzle->puzzle[last_index-1] = u;
    if (NOT_USP == check(inner_testing_puzzle)){
      local_skip[u] = true;
      continue;
    }
    not_skipped++;
  }

  bool ** graph = make_2d_bool_array(not_skipped);

  int u_i = -1;
  for (puzzle_row u = start_u; u < max_index; u++) {
    
    if (local_skip[u]) continue;
    u_i++;
    /*
    Start v at u+1 since it's an ordered puzzle
    */
    int v_i = -1;
    for (puzzle_row v = u+1; v < max_index; v++) {

      if (local_skip[v]) continue;
      v_i++;
      
      testing_puzzle->puzzle[last_index-1] = u;
      testing_puzzle->puzzle[last_index] = v;
      
      if (IS_USP == check(testing_puzzle)) {
	      // Edge present.
        graph[u_i][v_i] = true;
        graph[v_i][u_i] = true;
      } else {
	      // No edge present.
        graph[u_i][v_i] = false;
        graph[v_i][u_i] = false;
      }
    }
  }

  // printf("row col %d vs orig of %lu\n", not_skipped, max_index);
  
  // Update graph size.
  max_index = not_skipped;



  destroy_puzzle(inner_testing_puzzle);
  destroy_puzzle(testing_puzzle);

  if (which >= 0) {
    stop_graph_timer(heuristic_type, which);
  }

  return graph;
}

/*
 * Counts the connectedness of each vertex in a given graph
 * Returns a boolean representing whether or not any vertices at all exist
 */
bool count_vertices(puzzle_row vertex_counter[], bool ** graph, puzzle_row max_graph_row) {
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
bool remove_too_unconnected(puzzle_row test_clique, puzzle_row * vertices, bool ** graph, puzzle_row max_graph_index) {
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

int clique_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type) {

  // int k = p->k;
  puzzle_row max_index = ipow2(3, p->k);

  /* SET THE INITIAL BOUNDS TO SEARCH BETWEEN */
  puzzle_row curr_lower_bound = 0;
  puzzle_row curr_upper_bound = max_index;

  /* START SEARCHING ON ... */
  puzzle_row current_test_clique = (curr_lower_bound + curr_upper_bound) / 2;

  // Start at previous highest +1 since the puzzle is ordered
  puzzle_row next_start = p->puzzle[p->s-1] + 1; // start at the last element of the original puzzle + 1

  puzzle_row num_verts = max_index;
  
  bool ** graph = make_graph_from_puzzle(p, skip, next_start, num_verts, which, heuristic_type);

  if (num_verts == 0){
    free_2d_bool_array(graph, num_verts);
    return 0;
  }

  puzzle_row * good_vertices = (puzzle_row *) malloc(sizeof(puzzle_row) * num_verts);
  bzero(good_vertices, sizeof(puzzle_row) * num_verts);

  bool node_exists = count_vertices(good_vertices, graph, num_verts);

  puzzle_row * test_vertices = (puzzle_row *) malloc(sizeof(puzzle_row *) * num_verts);
  memcpy(test_vertices, good_vertices, sizeof(puzzle_row *) * num_verts);

  bool ** test_graph = make_2d_bool_array(num_verts);

  bool found = false;
  while (!found) {

    copy_2d_bool_array_contents(test_graph, graph, num_verts);

    // Reduce the graph to a fully connected graph
    bool changed = true;
    while (changed) {
      changed = remove_too_unconnected(current_test_clique, test_vertices, test_graph, num_verts);
    }

    // If most connected is now connected to 0, then the previous upper bound was the max max clique
    puzzle_row max = test_vertices[0];
    for (puzzle_row i = 0; i < num_verts; i ++) {
      if (test_vertices[i] > max) {
        max = test_vertices[i];
      }
    }

    if (max == 0) {
      curr_upper_bound = current_test_clique;
      memcpy(test_vertices, good_vertices, sizeof(puzzle_row *) * num_verts);
    } else {
      curr_lower_bound = current_test_clique;
      curr_upper_bound = MIN(curr_upper_bound, max);
      memcpy(test_vertices, good_vertices, sizeof(puzzle_row *) * num_verts);
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
  free_2d_bool_array(test_graph, num_verts);
  free_2d_bool_array(graph, num_verts);

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
