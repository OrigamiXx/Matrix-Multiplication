#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include <map>
/*
int main(int argc, char * argv[]){
  
  //puzzle * p = create_puzzle(8,6); //6,3); // correct one (8,6)
  //puzzle * p = create_puzzle_from_file("puzzles/test.puz");
  //print_puzzle(p);
  //if(check_usp_recursive(p)){
  //int i = check_usp(p);
  //printf("hello%d\n", i);
  //}

  
  //printf("result = %d\n",check_usp(p));
  //printf("%d\n",sizeof(int));
  //printf("%d\n",sizeof(long));
  //printf("%d\n",sizeof(long long));
  
  int row = 5;
  int column = 5;
  //int index = 1;
  //puzzle * p = create_puzzle_from_index(row,column,index);
  //print_puzzle(p);
  //check_all_usp(row, column);
  check_usp_same_col(row,column);
  
  return 0;

  }*/


// MapReduce random RMAT matrix generation example in C++
// Syntax: rmat N Nz a b c d frac seed {outfile}
//   2^N = # of rows in RMAT matrix
//   Nz = non-zeroes per row
//   a,b,c,d = RMAT params (must sum to 1.0)
//   frac = RMAT randomization param (frac < 1, 0 = no randomization)
//   seed = RNG seed (positive int)
//   outfile = output RMAT matrix to this filename (optional)

#include "mpi.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "mapreduce.h"
#include "keyvalue.h"
#include "usp.h"
#include "puzzle.h"
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
int max_poss_row =1;

void row_one_keys(int itask, KeyValue *kv, void *ptr){
   int i, row = 1;
  
  for(i = 0; i<max_poss_row; i++){
    kv->add((char*)&i, sizeof(int), NULL, 0);
  }
}

void extend_puzzle(uint64_t itask, char * key, int keybytes, char *value, int valuebytes, KeyValue *kv, void *ptr){
  int* puz = (int*)key;
  int new_puz[row];
  
  int i;
  for(i=0; i<row-1; i++){
    new_puz[i] = puz[i];
  }
  puzzle p;
  //printf("row = %d\n", row);
  //printf("column= %d\n", column);
  for(i = puz[row-2]; i < max_poss_row;i++){
    new_puz[row-1] = i;
    //puzzle p;
    p.row = row;
    p.column = column;
    p.puzzle = new_puz;
    p.pi = create_perm_identity(row);
    if (check_usp(&p)){//check_usp_recursive(&p)){
      //print_puzzle(&p);
      // printf("hello\n");
      kv->add((char*)new_puz, row*sizeof(int), NULL, 0);
    }
    destroy_perm(p.pi);
  }

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
  if (narg != 2 ){//&& narg != 10) {
    if (me == 0) printf("Syntax: usp_cluster k(column)");
      //printf("Syntax: rmat N Nz a b c d frac seed {outfile}\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }
  column = atoi(args[1]);
  if(column>19){
    if (me==0) printf("width can not be over 19.");
    MPI_Abort(MPI_COMM_WORLD,1);
  }
  for(int i = 0; i<column; i++){
    max_poss_row = max_poss_row*3;
  }
  MapReduce *mr = new MapReduce(MPI_COMM_WORLD);
  mr->verbosity = 0;
  mr->timer = 1;

  // loop until desired number of unique nonzero entries

  MPI_Barrier(MPI_COMM_WORLD);
  double tstart = MPI_Wtime();
  int count;
  count = mr->map(1, &row_one_keys, NULL, 1);
  if (me == 0) {
    printf("%d usps for the first row\n",count);
  }

  for(row = 2; row <= 4; row++){
    count =  mr->map(mr,&extend_puzzle, NULL);
    if (me == 0) {
    printf("%d usps for the first row\n",count);
    }
    mr->collate(NULL);
    mr->reduce(&cull,NULL);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  double tstop = MPI_Wtime();

  

  // stats to screen
  // include stats on number of nonzeroes per row

  

  if (me == 0)
    printf("time: %g secs\n",
	   tstop-tstart);

  // clean up

  delete mr;
  //delete [] rmat.outfile;
  MPI_Finalize();
}

