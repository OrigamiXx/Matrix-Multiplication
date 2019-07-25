
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <map>
#include <cstdlib>

// MapReduce finding usps to do matrix-multiplication
// Syntax: mpirun -np 4 search_para col
#include "permutation.h"
#include "constants.h"
#include "checker.h"
#include "mpi.h"
#include "math.h"

#include "mapreduce.h"
#include "keyvalue.h"
#include "puzzle.h"
#include "3DM_to_MIP.h"
#include "3DM_to_SAT.h"
#include "canonization.h"
#include "clique_to_mip.h"
#include "ExtensionGraph.hpp"

using namespace MAPREDUCE_NS;

bool check_isomorphs = true;
double check_isomorphs_threshold = 0.01;
int target = -1; // Target number of puzzles to pass between phases, -1 for all.
double prob;

void cull(char *, int, char *, int, int *, KeyValue *, void *);

void cull(char *key, int keybytes, char *multivalue,
	  int nvalues, int *valuebytes, KeyValue *kv, void *ptr)
{
  if (target <= 0 || drand48() <= prob){
    kv->add(key,keybytes,NULL,0);
  }
}


// NOTE: 32-bit vertex IDs, unlike in.rmat for OINK
// other formatting code below would need to be changed for 64-bit vertex IDs


int column;
int row;


void random_usps(int itask, KeyValue *kv, void *ptr){

  int * data = (int *)ptr;
  int count = 0;
  int target_rows = data[0];  // Number of rows in the search is targeting.
  int random_rows = data[1];  // Number of rows in each random puzzle.
  int random_tries = data[2]; // Number of random puzzles to to generate.
  int me = data[3];           // Process rank.

  /*
   * Generates a number of random target_rows-by-column puzzles.
   * Repeats until the number of puzzles where the first random_rows
   * of the puzzle is a strong USP is random_tries.  The resulting
   * puzzles are inserted in the KeyValue map kv for further
   * processing.  This is _not_ a uniform distribution over all
   * random_rows-by-column strong USPs, as it favors puzzles that are
   * more "compact".
   */
  long seed = time(NULL) + me * 997;
  printf("seed(%d) = %ld\n", me, seed);
  srand48(seed);
  puzzle * p = create_puzzle(target_rows, column);
  while (count < random_tries){

    randomize_puzzle(p);
    sort_puzzle(p);

    if (IS_USP == check(p)) {
			 //(IS_USP == check_SAT(p)){
			//(IS_USP == check_MIP(p)){
      count++;
      kv->add((char*)p -> puzzle, random_rows*sizeof(int), NULL, 0);
    }

  }

  destroy_puzzle(p);
}


void row_one_keys(int itask, KeyValue *kv, void *ptr){

  puzzle * p = create_puzzle(1,column);
  
  for(unsigned long i = 0; i < p -> max_row; i++){
    p -> puzzle[0] = i;
    if (!check_isomorphs || !have_seen_isomorph(p))
      kv->add((char*)&i, sizeof(puzzle_row), NULL, 0);
  }
   
}


void extend_puzzle_basic(uint64_t itask, char * key, int keybytes, char *value, int valuebytes, KeyValue *kv, void *ptr){

  puzzle * p = create_puzzle(row, column);
  
  memcpy(p -> puzzle, (puzzle_row *)key, sizeof(puzzle_row) * (row - 1));
    
  for(puzzle_row r = p -> puzzle[row-2] + 1; r < p -> max_row; r++){
    if (check(p) == IS_USP)
      kv->add((char*)(p -> puzzle), row * sizeof(puzzle_row), NULL, 0);
  }
  
  destroy_puzzle(p);
}

void extend_puzzle_extension(uint64_t itask, char * key, int keybytes, char *value, int valuebytes, KeyValue *kv, void *ptr){

  puzzle * p = create_puzzle(row -1, column);
  
  memcpy(p -> puzzle, (puzzle_row *)key, sizeof(puzzle_row) * (row - 1));

  ExtensionGraph * eg = new ExtensionGraph(p);
  
  // Helper function that sets heuristic result for every vertex in eg
  // to its degree.  No vertices are deleted.
  auto reduce_helper = [p, eg, kv](unsigned long index_u, unsigned long label_u, unsigned long degree_u) -> bool{

    puzzle * p2 = extend_puzzle(p, 1);
    p2 -> puzzle[p2 -> s - 1] = label_u;
    
    if (check_isomorphs)
      canonize_puzzle(p2);
    
    kv->add((char*)(p2 -> puzzle), p2 -> s * sizeof(puzzle_row), NULL, 0);

    destroy_puzzle(p2);
    
    return true;
  };

  // Computes the heuristic for each vertex.
  eg->reduceVertices(reduce_helper, false);

  destroy_puzzle(p);
}

void extend_puzzle(uint64_t itask, char * key, int keybytes, char *value, int valuebytes, KeyValue *kv, void *ptr){

  puzzle * p = create_puzzle(row-1, column);
  
  memcpy(p -> puzzle, (puzzle_row *)key, sizeof(puzzle_row) * (row - 1));
    
  for(puzzle_row r = 0; r < p -> max_row; r++){
    puzzle * p2 = create_puzzle_from_puzzle(p, r);
    if ((!check_isomorphs || !have_seen_isomorph(p2)) && check(p2) == IS_USP){
      if (check_isomorphs)
	canonize_puzzle(p2);
      kv->add((char*)(p2 -> puzzle), p2 -> s * sizeof(puzzle_row), NULL, 0);
    }
    destroy_puzzle(p2);
  }
  
  destroy_puzzle(p);
}

/* ---------------------------------------------------------------------- */

int main(int narg, char **args)
{
  MPI_Init(&narg,&args);
  srand48(time(NULL));
  
  int me,nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  // parse command-line args
  //int column;
  if (narg != 2 && narg != 5){//&& narg != 10) {
    if (me == 0) fprintf(stderr, "Syntax: search_para <column> [target_rows] [random_rows] [random_tries]\n");
    MPI_Abort(MPI_COMM_WORLD,1);
    return 0;
  }
  column = atoi(args[1]);
  if(column>19){
    if (me==0) printf("width can not be over 19.");
    MPI_Abort(MPI_COMM_WORLD,1);
  }
  MapReduce *mr = new MapReduce(MPI_COMM_WORLD);
  mr->verbosity = 0;
  mr->timer = 1;

  // loop until desired number of unique nonzero entries

  MPI_Barrier(MPI_COMM_WORLD);
  double tstart = MPI_Wtime();
  int count;

  if (narg == 2){
    count = mr->map(1, &row_one_keys, NULL, 1);
    if (me == 0) {
      printf("%d usps for the first row\n",count);
    }
    row = 2;
  } else {
    int target_rows = atoi(args[2]);
    int random_rows = atoi(args[3]);
    int random_tries = atoi(args[4]);
    int data[4] = {target_rows, random_rows, random_tries / nprocs, me};
    count = mr->map(nprocs, &random_usps, data);
    if (me == 0) {
      printf("Starting with %d random (%d,%d) strong USPs.\n",count,random_rows,column);
    }
    row = random_rows + 1;
  }
  while(count != 0){

    int map_count = mr->map(mr,&extend_puzzle, NULL);
    int col_count = mr->collate(NULL);
    double ave_branching = (double)col_count / count; 
    prob = (double)target / (double)col_count;
    count = mr->reduce(&cull,NULL);


    double tnow = MPI_Wtime();
    if(me == 0){
      printf("Cumulative time (secs): %g secs\n", tnow-tstart);
    }
    
    if (me == 0) {
      printf("%d <= %d (%.2fx) <= %d usps for row %d\n", count, col_count, ave_branching, map_count, row);
    }
    
    /* if (check_isomorphs && (double)(map_count - col_count) / map_count < check_isomorphs_threshold){ */
    /*   if (me == 0) */
    /* 	printf("===>>> Checking isomorphs disabled <<<===\n"); */
    /*   check_isomorphs = false; */
    /* } */

    if (me == 0)
      printf("\n");
    row++;
    
    reset_isomorphs();
    //row++;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  double tstop = MPI_Wtime();

  // stats to screen
  // include stats on number of nonzeroes per row
  //finalize_check_MIP();

  if (me == 0)
    printf("time: %g secs\n", tstop-tstart);

  // clean up

  delete mr;
  //delete [] rmat.outfile;
  MPI_Finalize();
}
