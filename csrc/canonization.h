/*
 * Module implementing puzzle canonization using Nauty.
 *
 * Author: Matt.
 */


#include "puzzle.h"

#define MAX_ISOMORPHS -1

// Returns true iff no isomorphs of p have been previously seen.
bool have_seen_isomorph(puzzle * p, bool remember = true, int * index = NULL);

// Reset the set of previously seen isomorphs.
void reset_isomorphs();

// Returns the number of previously seen isomorphs.
size_t get_num_isomorphs();

// Replace p with its canonical isomorph.
void canonize_puzzle(puzzle * p);

// Returns true iff p1 and p2 are isomorphs.
bool are_isomorphs(puzzle * p1, puzzle * p2);

//Stores the set of previously seen isomorphs. After a set has been stored, a new set can not be stored.
void store_state();

//Resets the set of seen isomorphs to the previously stored set.
void revert_stored_state();
