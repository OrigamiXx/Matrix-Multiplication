#ifndef __SEARCHER_H__
#define __SEARCHER_H__

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <queue>
#include <string>

#include "constants.h"
#include "checker.h"
#include "puzzle.h"
#include "searcher.h"

struct heuristic_result {
  int result;
  puzzle_row value;

  bool operator<(const heuristic_result & rhs) const 
  {
    return result < rhs.result;
  }
};

struct cumulative_counts {
  struct timespec start_time;
  bool is_timing;

  struct timespec graph_start_time;
  bool graph_is_timing;


  double graph_time_at_level;
  double total_time_at_level;
  int total_instances_at_level;

  int num_heuristics;
  double total_heuristic;
  double max_heuristic;
  double min_heuristic;

};

struct cumulative_tracker {
  int num_details;
  cumulative_counts * details;
};



int ipow2(int base, int exp);
bool ** make_2d_bool_array(unsigned long max_index);
void copy_2d_bool_array_contents(bool ** dest, bool ** src, unsigned long max_index);
void free_2d_bool_array(bool ** arr_2d, unsigned long max_index);
bool count_vertices(unsigned long vertex_counter[], bool ** graph, puzzle_row max_graph_row);
bool ** make_graph_from_puzzle(puzzle * p, bool skip[], puzzle_row start_u, puzzle_row max_u, puzzle_row max_v, int which, int heuristic_type);

int nullity_search(puzzle * p, bool skip[], int skip_count, int best, int which, int heuristic_type);
int search(puzzle * p, bool skip[], int skip_count, int best, int which, int heuristic_type, int stop_at_s, bool stats_enabled);
int generic_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type, int stop_at_s);
int inline_search(puzzle * p, bool ** graph, bool skip[], int skip_count, int best, int which, int heuristic_type, int stop_at_s);
void inline_h(puzzle * p, bool ** graph, std::priority_queue<heuristic_result> * h_queue, bool skip[], int skip_count, int which, int heuristic_type);
int nullity_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type, int stop_at_s);
int brute_force(puzzle * p, bool skip[], int skipping, int up_to_size);

int clique_h(puzzle *p, bool skip[], int skip_count, int which, int heuristic_type); 




#endif