/*
 * Module implementing puzzle canonization using Nauty.
 *
 * Author: Matt.
 */


#include "puzzle.h"

#define MAX_ISOMORPHS -1

// Returns true iff no isomorphs of p have been previously seen.
bool have_seen_isomorph(puzzle * p);

// Replace p with its canonical isomorph.
void canonize_puzzle(puzzle * p);
