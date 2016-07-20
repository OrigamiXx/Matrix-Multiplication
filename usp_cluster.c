#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include <map>
#include <cstdlib>

// MapReduce finding usps to do matrix-multiplication
// Syntax: mpirun -np 4 ./usp_cluster column(column number)
#include "mpi.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "mapreduce.h"
#include "keyvalue.h"
#include "usp.h"
#include "puzzle.h"
#include "usp_bi.h"
#include  <time.h>
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
   /*//insert one specific puzzle
   string raw_input = "puzzles/test.puz";
   const char * input = raw_input.c_str();
   puzzle * p = create_puzzle_from_file(input);
   if (check_usp_recursive(p)){
     kv->add((char*)p->puzzle,p->row*sizeof(int),NULL,0);	
   }
     destroy_puzzle(p);*/
   //insert all row one puzzle in the column size
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
  //int index = 0;
  int percent;
  //printf("row = %d\n", row);
  //printf("column= %d\n", column);
  int seed = 1;
  srand(time(NULL));
  for(i = puz[row-2]; i < max_poss_row;i++){
    new_puz[row-1] = i;
    //puzzle p;
    p.row = row;
    p.column = column;
    p.puzzle = new_puz;
    p.pi = create_perm_identity(row);
    
    percent = rand() % 100;
    //percent = 0;
    int percentage = 55;
    //printf("%d\n",percent);
    if (check(p.puzzle, p.row, p.column)&&percent<percentage){//check_usp(&p)){//check_usp_recursive(&p)){
      //index++;
      //if(index<2){
      
      kv->add((char*)new_puz, row*sizeof(int), NULL, 0);
      //}
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
  row = 2;
  while(count != 0){
    count =  mr->map(mr,&extend_puzzle, NULL);
    if (me == 0) {
      printf("%d usps for row %d\n",count, row);
    }
    mr->collate(NULL);
    mr->reduce(&cull,NULL);
    row++;
    double tnow = MPI_Wtime();
    if(me == 0){
      printf("this program has run: %g secs\n", tnow-tstart);
    }
    //row++;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  double tstop = MPI_Wtime();

  

  // stats to screen
  // include stats on number of nonzeroes per row

  

  if (me == 0)
    printf("time: %g secs\n", tstop-tstart);

  // clean up

  delete mr;
  //delete [] rmat.outfile;
  MPI_Finalize();
}

