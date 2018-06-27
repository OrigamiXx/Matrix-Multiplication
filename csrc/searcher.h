#ifndef __SEARCHER_H__
#define __SEARCHER_H__

#include <stdio.h>
#include <stdlib.h>

#include "checker.h"
#include "puzzle.h"

int nullity_search(puzzle * p, bool skip[], int skip_count, int best, int which, int heuristic_type);
int search(puzzle * p, bool skip[], int skip_count, int best, int which, int heuristic_type);
int generic_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type);
int nullity_h(puzzle * p, bool skip[], int skip_count, int which, int heuristic_type);
int brute_force(puzzle * p, bool skip[], int skipping, int up_to_size);

int clique_h(puzzle * p, int entries_per_vertex);

#endif