
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <map>
#include <cstdlib>

// MapReduce finding usps to do matrix-multiplication
// Syntax: mpirun -np 4 ./usp_cluster column(column number)
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

using namespace MAPREDUCE_NS;


void cull(char *, int, char *, int, int *, KeyValue *, void *);

void cull(char *key, int keybytes, char *multivalue,
	  int nvalues, int *valuebytes, KeyValue *kv, void *ptr)
{
  kv->add(key,keybytes,NULL,0);
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
    if (!have_seen_isomorph(p))
      kv->add((char*)&i, sizeof(puzzle_row), NULL, 0);
  }
   
}

void extend_puzzle(uint64_t itask, char * key, int keybytes, char *value, int valuebytes, KeyValue *kv, void *ptr){

  puzzle * p = create_puzzle(row -1, column);
  
  memcpy(p -> puzzle, (puzzle_row *)key, sizeof(puzzle_row) * (row - 1));
    
  for(puzzle_row r = 0 /* p -> puzzle[row-2] + 1*/; r < p -> max_row; r++){

    puzzle * p2 = create_puzzle_from_puzzle(p, r);
    canonize_puzzle(p2);
    
    int percentage = 100;
    if (rand() % 100 < percentage && check(p2) == IS_USP)
      //if (!have_seen_isomorph(p2))
	kv->add((char*)(p2 -> puzzle), row * sizeof(puzzle_row), NULL, 0);

    destroy_puzzle(p2);
  }
  
  destroy_puzzle(p);
}
/* ---------------------------------------------------------------------- */

int main(int narg, char **args)
{
  MPI_Init(&narg,&args);

  int me,nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  // parse command-line args
  //int column;
  if (narg != 2 && narg != 5){//&& narg != 10) {
    if (me == 0) fprintf(stderr, "Syntax: usp_para <column> [target_rows] [random_rows] [random_tries]\n");
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
    mr->map(mr,&extend_puzzle, NULL);
    count = mr->collate(NULL);
    mr->reduce(&cull,NULL);
    if (me == 0) {
      printf("%d usps for row %d\n",count, row);
    }

    row++;
    double tnow = MPI_Wtime();
    if(me == 0){
      printf("this program has run: %g secs\n", tnow-tstart);
    }
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
