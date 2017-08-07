
#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"
#include <string.h>
#include "constants.h"
#include "assert.h"
#include <math.h>
#include "matrix.h"
#include "puzzle.h"
#include <algorithm>
#include "usp.h"





puzzle * create_puzzle(int s, int k){

  puzzle * p = (puzzle *) (malloc(sizeof(puzzle)));

  // Initialize dimensions of puzzle.
  p -> puzzle = (puzzle_row *) (malloc(sizeof(puzzle_row) * s));
  p -> max_row = MAX_ROWS[k];
  p -> s = s;
  p -> k = k;
  p -> tdm = (bool *) (malloc(sizeof(bool) * s * s * s));

  bzero(p -> puzzle, sizeof(puzzle_row) * s);

  return p;
}

//create a puzzle that has one more row and same width as the input puzzle
// according to the given row_index
puzzle * create_puzzle_from_puzzle(puzzle * p, puzzle_row row){

  puzzle * result = create_puzzle(p -> s + 1, p -> k);

  memcpy(result -> puzzle, p -> puzzle, sizeof(puzzle_row) * p -> s);
  result -> puzzle[result -> s - 1] = row;
  
  return result;
}

//read a puzzle from a file.
puzzle * create_puzzle_from_file(const char * filename){

  puzzle * p = (puzzle *) (malloc(sizeof(puzzle)));

  FILE * f = fopen(filename,"r");

  if (f == NULL)
    return NULL;

  char buff[256];

  int element;
  //first check whether this file is able to turn into a puzzle
  int bytes_read = fscanf(f,"%s\n",buff);
  assert(bytes_read > 0);

  unsigned int k = strlen(buff);

  int s = 1;
  if (buff[0] == '#') s = 0;
  //loop until end of file using feof(f).  Build up puzzle DS.
  while(!feof(f)){
    bytes_read = fscanf(f,"%s\n",buff);
    assert(bytes_read > 0);
    if (buff[0] == '#') continue; // Comment
    if (k != strlen(buff)){
      fprintf(stderr, "Error: Puzzle rows not all same length.\n");
      return NULL;
    }
    for(unsigned int i = 0; i < k; i++){
      element = buff[i] - '0';
      if(element != 1 &&  element != 2 && element != 3){
	fprintf(stderr, "Error: Puzzle entries can only be 1 or 2 or 3.\n");
	return NULL;
      }
    }
    s++;
  }
  
  p = create_puzzle(s, k);

  f = fopen(filename,"r");
  int r = 0;
  while(!feof(f)){
    bytes_read = fscanf(f,"%s\n", buff);
    assert(bytes_read > 0);
    if (buff[0] == '#') continue;
    p -> puzzle[r] = atol(buff);
    r++;
  }
  return p;
}

//Return the next available puzzle from the open file f
puzzle * next_file(FILE * f, int max_rows){

  if (f == NULL)
    return NULL;
  
  char buff[256];
  char * result;

  //first check whether this file is able to turn into a puzzle
  result = fgets(buff, 30, f);
  assert(result != NULL);

  int k = strlen(buff);
  puzzle * p = create_puzzle(max_rows, k);
  int r = 0;

  //Loop until encountered an empty line
  while(buff[0] != '\n'){
    p -> puzzle[r] = atol(buff);
    r++;
    result = fgets(buff, 30, f);
    assert(result != NULL);
  }
  p -> s = r;
  return p;
}




//give width(column) and height(row) and the index of all the possible
//puzzle in this size return a puzzle index must be 0-(3^(k*s)-1)
puzzle * create_puzzle_from_index(int s, int k, puzzle_row index){
  
  puzzle * p = create_puzzle(s, k);

  for(int r = 0; r < s; r++){
    p -> puzzle[r] = index % p -> max_row;
    index /= p -> max_row;
  }
  
  return p;
}

// with a given puzzle return the index out of 0-(3^(r*c)-1)
int get_index_from_puzzle(puzzle * p){

  int result = p -> puzzle[p -> s - 1];
  for (int i = p -> s - 2; i >= 0; i--){
    result = result * p -> max_row + p -> puzzle[i];
  }
  return result;
}

// Replaces data in puzzle with an random puzzle of the same
// dimensions.
void randomize_puzzle(puzzle * p){

  for (int i = 0 ; i < p -> k; i++) 
    p -> puzzle[i] = lrand48() % MAX_ROWS[p -> k];

}

// Sets the given puzzle p to a random strong USP of the same size.
// May not return if a strong USP of the given size does not exist.
// Is likely very slow except for small puzzle sizes.
void random_usp(puzzle * p){

  randomize_puzzle(p);
  while (!check(p)){
    randomize_puzzle(p);
  }

}

// Sorts the rows of the puzzle in increasing order.
void sort_puzzle(puzzle * p){

  sort(p -> puzzle, p -> puzzle + p -> s);

}

void arrange_puzzle(puzzle * p){

  int balance[p -> k] = {0};
  
  for (int c = 0; c < p -> k; c++){
    int counts[3] = {0,0,0};
    for (int r = 0; r < p -> s; r++){
      counts[get_entry(p, r, c) - 1]++;
    }

    if (counts[0] == 0){
      balance[c] = abs(counts[1] - counts[2]);
    } else if (counts[1] == 0){
      balance[c] = abs(counts[0] - counts[2]);
    } else if (counts[2] == 0){
      balance[c] = abs(counts[0] - counts[1]);
    } else {
      balance[c] = -1;
    }
    
  }

  for (int c = 0; c < p -> k ; c++){
    int max = balance[c];
    int max_c = c;
    for (int c1 = c + 1; c1 < p -> s ; c1++){
      if (max > balance[c1]) {
	max = balance[c1];
	max_c = c1;
      }
    }

    balance[max_c] = balance[c];
    for (int r = 0; r < p -> s; r++){
      int tmp = get_entry(p, r, max_c);
      set_entry(p, r, max_c, get_entry(p, r, c));
      set_entry(p, r, c, tmp);
    }
    
  }
  
  sort(p -> puzzle, p -> puzzle + p -> s);

}


// print a puzzle
void print_puzzle(puzzle * p){
  if (p != NULL){
    int r,c;
    for(r = 0; r < p -> s; r++){
      for(c = 0; c < p -> k; c++){
	printf("%d", get_entry(p, r, c));
      }
      printf("\n");
    }
  }
}

void print_tdm(puzzle * p){
  int s = p -> s;
  for (int r1 = 0; r1 < s; r1++){
    for (int r2 = 0; r2 < s; r2++){
      for (int r3 = 0; r3 < s; r3++){
	printf("%d", p -> tdm[r1 * s * s + r2 * s + r3]);
      }
      printf("\n");
    }
    printf("\n");
  }
}

/*
 * Returns true iff a length-k row that permutations map to u_1, u_2,
 * u_3, respectively, satisfies the inner condition of strong USPs.
 * It is false if this length-k row this mapping does not witness that
 * the puzzle is a strong USP.  Runtime is O(k).
 */
bool is_witness(puzzle * p, int r1, int r2, int r3){

  puzzle_row u1 = p -> puzzle[r1];
  puzzle_row u2 = p -> puzzle[r2];
  puzzle_row u3 = p -> puzzle[r3];
  
  for (int i = 0; i < p -> k; i++){
    int count = 0;
    puzzle_row d1 = u1 % 3;
    puzzle_row d2 = u2 % 3;
    puzzle_row d3 = u3 % 3;
    u1 = (u1 - d1) / 3;
    u2 = (u2 - d2) / 3;
    u3 = (u3 - d3) / 3;

    if (d1 == 0) count++;
    if (d2 == 1) count++;
    if (d3 == 2) count++;
    if (count == 2) return true;
  }
  return false;
}

void compute_tdm(puzzle * p){

  int s = p -> s;
  for (int r1 = 0; r1 < s; r1++){
    for (int r2 = 0; r2 < s; r2++){
      for (int r3 = 0; r3 < s; r3++){
	p -> tdm[r1 * s * s + r2 * s + r3] = !is_witness(p, r1, r2, r3);
      }
    }
  }
  
}


void simplify_tdm(puzzle * p){

  int s = p -> s;
  int k = p -> k;
  
  for (int c = 0; c < k; c++){
    int counts[3] = {0,0,0};
    for (int r = 0; r < s; r++){
      counts[get_entry(p, r, c) - 1]++;
    }

    int missing = (counts[0] == 0) + (counts[1] == 0) + (counts[2] == 0);
    if (missing == 1) {

      for (int i = 0; i < s; i++){
	for (int j = 0; j < s; j++){
	  if (get_entry(p, i, c) != get_entry(p, j, c)) {
	    for (int l = 0; l < s; l++){
	      if (counts[0] == 0) {
		p -> tdm[l * s * s + i * s + j] = true;
		p -> tdm[l * s * s + j * s + i] = true;
	      } else if (counts[1] == 0){
		p -> tdm[i * s * s + l * s + j] = true;
		p -> tdm[j * s * s + l * s + i] = true;
	      } else {
		p -> tdm[i * s * s + j * s + l] = true;
		p -> tdm[j * s * s + i * s + l] = true;
	      }
	    }
	  }
	}
      }
    }

  }

}

int count_tdm(puzzle * p){

  int count = 0;
  int s = p -> s;
  for (int r1 = 0; r1 < s; r1++){
    for (int r2 = 0; r2 < s; r2++){
      for (int r3 = 0; r3 < s; r3++){
	if (p -> tdm[r1 * s * s + r2 * s + r3])
	  count++;
      }
    }
  }
  return count;
}



// Deallocates a puzzle
void destroy_puzzle(puzzle * p){

  if (p != NULL){
    if (p -> tdm != NULL)
      free(p -> tdm); 
    free(p -> puzzle);
    free(p);
  }
  
}
