#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <map>
#include <math.h>
#include <assert.h>
#include <time.h>

#include "permutation.h"
#include "constants.h"
#include "checker.h"
#include "heuristic.h"
#include "mpi.h"
#include "math.h"

#include "mapreduce.h"
#include "keyvalue.h"
#include "puzzle.h"

using namespace std;
using namespace MAPREDUCE_NS;

// ===================================================================================================
//
// Parameters
//
// ===================================================================================================

#define MEMSIZE 2048

// Initialization Options
int starting_s = 2;
int usp_cache_size = 2;

// Modes:
// 1 - Naive
// 2 - Lookup
// 3 - Serial lookup
// 4 - Serial lookup and map
int mode = 3;  

// Mode specific options
// . Mode 1 - Naive
puzzle_row max_start_row = 1000000; 

// . Mode 4 - Serial lookup and map
int map_serial_stride = 1;

// Global variables.
puzzle_row max_row = -1;
int k = -1;
int me,nprocs;
double t_total = 0.0;

puzzle_row init_row = 0;
bool prune_start_row = false;
bool greedy = true;
double greedy_count = 2;
//int max_scan = 100;

// Map Reduce Option Initialization.
void MR_set_opt(MapReduce *mr){

  mr -> verbosity = 0;
  mr -> timer = 0;
  mr -> outofcore = -1;
  mr -> freepage = 1;
  mr -> mapstyle = 0;
  mr -> memsize = MEMSIZE;
  mr -> zeropage = 0;

}

// ===================================================================================================
//
// Initialization Callbacks
//
// ===================================================================================================

void generate_init_usps(int itask, KeyValue *kv, void *ptr){

  kv -> add((char *)ptr,sizeof(puzzle_row),NULL,0);

}


// ===================================================================================================
//
// Map Callbacks
//
// ===================================================================================================

void map_naive(uint64_t itask, char *key, int keybytes, char *value, int valuebytes, KeyValue *kv, void *ptr){

  int s = keybytes / sizeof(puzzle_row) + 1;
  puzzle * p = create_puzzle(s, k);
  memcpy(p -> puzzle, key, keybytes);
  puzzle_row * puz = p -> puzzle;
  
  int newbytes = keybytes + sizeof(puzzle_row);
  
  int count = 0;
  //int scanned = 0;
  for (puz[s-1] = puz[s-2]+1;  puz[s-1] < p -> max_row; puz[s-1]++) {

    // scanned++;
    // if (scanned >= max_scan)
    //   break;

    if (heuristic_row_triples(p) == NOT_USP) continue;

    if (check(p) == IS_USP) {
      //scanned = 0;
      kv -> add((char*)puz, newbytes, NULL, 0);
      if (greedy) {
	count++;
	if (count >= greedy_count)
	  break;
	if ((greedy_count - count < 1.0) && (drand48() > (greedy_count - count)))
	  break;
      }
    }

  }

  destroy_puzzle(p);
}



void map_lookup(uint64_t itask, char *key, int keybytes, char *value, int valuebytes, KeyValue *kv, void *ptr){

  int s = keybytes / sizeof(puzzle_row) + 1;
  puzzle * p = create_puzzle(s, k);
  memcpy(p -> puzzle, key, keybytes);
  puzzle_row * puz = p -> puzzle;
  
  int newbytes = keybytes + sizeof(puzzle_row);

  puzzle_row subset[s-1];

  for (puz[s-1] = puz[s-2] + 1;  puz[s-1] < p -> max_row; puz[s-1]++){

    //if (heuristic_row_triples(puzzle) == NOT_USP) continue;

    for (int i = 1; i < s - 1; i++){ // Must include first row to enable lookup.
      int curr = 0;

      for (int j = 0; j < s; j++){
	if (j == i) continue;
 	subset[curr] = puz[j];
	curr++;
      }

      kv -> add((char*)subset, keybytes, (char *)puz, newbytes);

    }
  }

  kv -> add(key, keybytes, NULL, 0);

  destroy_puzzle(p);

}

void map_serial_lookup(uint64_t itask, char *key, int keybytes, char *value,
		       int valuebytes, KeyValue *kv, void *ptr){

  int s = keybytes / sizeof(puzzle_row);
  puzzle * p = create_puzzle(s, k);
  puzzle_row * puz = p -> puzzle;
  
 
  // For batch skip the second row instead of the first, otherwise we can't look up.
  puz[0] = ((puzzle_row *)key)[0];
  for (int i = 1; i < s - 1; i++){
    puz[i] = ((puzzle_row *)key)[i+1];
  }

  int added = 0;
  int count = 0;
  for (puz[s-1] = puz[s-2] + 1;  puz[s-1] < p -> max_row; puz[s-1]++){

    count++;

    if (heuristic_row_triples(p) == NOT_USP) continue;
    
    kv -> add((char*)puz, keybytes, key + sizeof(puzzle_row), sizeof(puzzle_row));
    added++;

  }

  kv -> add(key, keybytes, NULL, 0);

  destroy_puzzle(p);

}

void map_serial_map_and_lookup(uint64_t itask, char *key, int keybytes, char *value, int valuebytes, KeyValue *kv, void *ptr){

  int s = keybytes / sizeof(puzzle_row);
  puzzle * p = create_puzzle(s, k);
  puzzle_row * puz = p -> puzzle;
  
  for (int i = 0; i < s - 1; i++){
    puz[i] = ((puzzle_row *)key)[i+1];
  }

  puzzle_row next_row = *(puzzle_row *)ptr;

  for (int i = 0; i < map_serial_stride; i++){
    if (next_row - i > puz[s-2]) {
      puz[s-1] = next_row - i;
      kv -> add((char*)puz, keybytes, key, sizeof(puzzle_row));
    }
  }    

  kv -> add(key, keybytes, NULL, 0);

  destroy_puzzle(p);

}



// ===================================================================================================
//
// Reduce Callbacks
//
// ===================================================================================================


void pass_on(uint64_t itask, char *key, int keybytes, char *value, int valuebytes, KeyValue *kv, void *ptr){

  KeyValue * pass_kv = (KeyValue *)ptr;

  pass_kv -> add(key, keybytes, value, valuebytes);

}



void reduce_serial_lookup(char *key, 
	       int keybytes, char *multivalue, 
	       int nvalues, int *valuebytes, 
	       KeyValue *kv, void *ptr) {


  int s = keybytes / sizeof(puzzle_row);
  int curr_row = *((int *)ptr);

  bool found_usp = false;

  for (int i = 0; i < nvalues && !found_usp; i++){
    if (valuebytes[i] == 0)
      found_usp = true;
  }

  if (!found_usp)
    return;

  int offset = 0;

  if (curr_row != s - 1){
  
    puzzle_row puzzle[s];
    memcpy(puzzle,key,keybytes);
    puzzle_row next_row = puzzle[curr_row];
    
    for (int i = 0; i < nvalues; i++){
      
      if (valuebytes[i] != 0) {
	puzzle[curr_row] = *(puzzle_row *)(multivalue+offset);
	kv -> add((char *)puzzle,keybytes,(char *)&next_row,sizeof(puzzle_row));
      } else {
	kv -> add(key,keybytes,NULL,0);
      }
      offset += valuebytes[i];
    }    
    
  } else {

    puzzle * p = create_puzzle(s+1, k);
    memcpy(p -> puzzle, key, keybytes);
    puzzle_row * puz = p -> puzzle;
    puz[s] = puz[s-1];
    
    for (int i = 0; i < nvalues; i++){

      if (valuebytes[i] != 0) {

	puz[s-1] = *(puzzle_row *)(multivalue + offset);
	if (check(p) == IS_USP)
	  kv -> add((char *)puz, keybytes + sizeof(puzzle_row), NULL, 0);
      } 
      offset += valuebytes[i];

    }

    destroy_puzzle(p);
  }

}



void reduce_naive(char *key, 
	       int keybytes, char *multivalue, 
	       int nvalues, int *valuebytes, 
	       KeyValue *kv, void *ptr) {

  
  kv -> add(key,keybytes,multivalue,valuebytes[0]);
}



void reduce_lookup_1(char *key, 
		     int keybytes, char *multivalue, 
		     int nvalues, int *valuebytes, 
		     KeyValue *kv, void *ptr) {

  bool found_usp = false;

  for (int i = 0; i < nvalues && !found_usp; i++){
    if (valuebytes[i] == 0)
      found_usp = true;
  }

  if (!found_usp)
    return;

  int value = 1;
  int offset = 0;
  for (int i = 0; i < nvalues; i++){
    if (valuebytes[i] != 0)
      kv -> add(multivalue+offset,valuebytes[i],(char *)&value,sizeof(int));
    offset += valuebytes[i];
  }    

}


void reduce_lookup_2(char *key, 
		     int keybytes, char *multivalue, 
		     int nvalues, int *valuebytes, 
		     KeyValue *kv, void *ptr) {

  int s = keybytes / sizeof(puzzle_row);

  if (nvalues == s-2) { // This is the number that must be looked up.  It's one less for batch.

    puzzle * p = create_puzzle(s, k);
    memcpy(p -> puzzle, key, keybytes);
    
    if (check(p) == IS_USP)
      kv -> add(key,keybytes,NULL,0);

    destroy_puzzle(p);
  }

}

// ===================================================================================================
//
// Print Callbacks
//
// ===================================================================================================

void print_row(FILE * fp, puzzle_row U, int k){
  
  int x = U;
  for (int j = 0; j < k; j++){
    fprintf(fp,"%d ",x % 3 + 1);
    x = (x - (x % 3)) / 3;
  }
}

void fprintUKV(FILE * fp, char * key, int keybytes, char * value, int valuebytes){
  
  puzzle_row * U = (puzzle_row *)key;
  int s = keybytes / sizeof(puzzle_row);

  for (int i =0; i < s; i++){
    print_row(fp,U[i],k);
    fprintf(fp,"\n");
  }
  fprintf(fp,"---\n");

}




// ===================================================================================================
//
// Algorithms
//
// ===================================================================================================


uint64_t do_naive(MapReduce ** mr_ptr, int s){

  uint64_t curr_count = 0;
  MapReduce *mr = *mr_ptr;

  mr -> map(mr, &map_naive,NULL);
  curr_count = mr -> collate(NULL);
  curr_count = mr -> reduce(&reduce_naive,NULL);
  
  return curr_count;
}

uint64_t do_lookup(MapReduce ** mr_ptr, int s){

  uint64_t curr_count = 0;
  MapReduce *mr = *mr_ptr;

  curr_count = mr -> map(mr, &map_lookup,NULL);
  curr_count = mr -> collate(NULL);
  curr_count = mr -> reduce(&reduce_lookup_1,NULL);
  curr_count = mr -> collate(NULL);
  curr_count = mr -> reduce(&reduce_lookup_2,NULL);
	
  return curr_count;
}

uint64_t do_serial_lookup(MapReduce ** mr_ptr, int s){

  uint64_t curr_count = 0;
  MapReduce *mr = *mr_ptr;
  
  mr -> map(mr, &map_serial_lookup,NULL);
  for (int i = 1; i <= s-2; i++){
    curr_count = mr -> collate(NULL);
    //if(me == 0)
    //  printf("i = %d, count = %ld\n",i,curr_count);
    curr_count = mr -> reduce(&reduce_serial_lookup,(void *)&i);
    //if(me == 0)
    //  printf("i = %d, count = %ld\n",i,curr_count);
  }
  
  return curr_count;
}

uint64_t do_serial_map_and_lookup(MapReduce ** mr_ptr, int s){

  uint64_t curr_count = 0;
  MapReduce *mr = *mr_ptr;

  MapReduce *mr_tmp = new MapReduce(MPI_COMM_WORLD);
  MR_set_opt(mr_tmp);
  curr_count = 0;
  uint64_t count = 0;
  
  if (me == 0) {
    
    printf("\r%6.2f%%\t%8.2f (ETA mins)\t (%d,%d) strong USPs found = 0", 0.0,1/0.0, s,k);
    fflush(stdout);
  }
  
  //map_serial_stride /= 3;
  //if (map_serial_stride < 1)
  map_serial_stride = 1;
  
  for (puzzle_row row = max_row - 1;  row >= 0; row-=map_serial_stride){
    
    MapReduce *mr_one_row = new MapReduce(MPI_COMM_WORLD);
    MR_set_opt(mr_one_row);
    //mr_one_row -> freepage = 0;
    
    mr_one_row -> map(mr, &map_serial_map_and_lookup,(void *)&row,1);
    for (int i = 0; i <= s-2; i++){
      count = mr_one_row -> collate(NULL);
      count = mr_one_row -> reduce(&reduce_serial_lookup,(void *)&i);
    }

    //mr_tmp -> open();
    // Pass KV off to mr_tmp instead of creating destroying mr_one_row.
    //mr_one_row -> map(mr_one_row, &pass_on,(void *)(mr_tmp -> kv));
    //count = mr_tmp -> close();
    mr_tmp -> add(mr_one_row);
    
    curr_count += count;
    
    // if (me == 0) {
    //   double t_curr = MPI_Wtime();
    //   double t_elapsed = t_curr - t_start;
    //   double ETA = t_elapsed * (double)max_row / (double)(max_row - row) - t_elapsed;
      
    //   printf("\r%6.2f%%\t%8.2f (ETA mins)\t (%d,%d) strong USPs found = %ld", 
    // 	     (double)(max_row - row) / (double)max_row * 100,ETA / 60, s,k,curr_count);
    //   fflush(stdout);
    // }

    delete mr_one_row;
    
  }	 
  
  delete mr;
  *mr_ptr = mr_tmp;

  return curr_count;

}


// ===================================================================================================
//
// Main Function
//
// ===================================================================================================

bool is_row_sorted(puzzle_row r, int k){

  int counts[3] = {0,0,0};
  puzzle_row x = r;
  int prev = 1000;
  for (int j = 0; j < k; j++){
    int curr = x % 3;
    counts[curr]++;
    if (prev < curr || (prev != 1000 && prev > curr + 1))
      return false;
    x = (x - (x % 3)) / 3;
    prev = curr;
  }
  if (prev != 0)
    return false;
  if (counts[1] > counts[0] || counts[2] > counts[1])
    return false;

  return true;

}

int main(int narg, char **args)
{

  // ---------------- Initialization ---------------------

  MPI_Init(&narg,&args);

  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  // parse command-line args
  if (narg != 2) {
    if (me == 0) printf("Syntax: usp_mapreduce k\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  k = atoi(args[1]);
  max_row = (puzzle_row)pow(3,k);

  MPI_Barrier(MPI_COMM_WORLD);
  double t_start = MPI_Wtime();
  double t_global = t_start;

  time_t t1;
  
  time(&t1);
  srand48((long) t1);

  init_cache(k,usp_cache_size);

  MPI_Barrier(MPI_COMM_WORLD);
  double t_stop = MPI_Wtime();
  
  if (me == 0){
    printf("\rCached up to size %d in %g secs.                    \n",
	   usp_cache_size, t_stop - t_start);
    fflush(stdout);
  }
 
  int max_s = 0;

  //for (puzzle_row start_row = init_row; start_row >= 0; start_row--){ // Reverse scan
  for (puzzle_row start_row = init_row; start_row < max_row; start_row++){ // Forward scan
    
    if (prune_start_row && !is_row_sorted(start_row,k)) {
      // Only need to consider unique permutation of first row.
      //if (me == 0)
      //   printf("skipping.\n");
      continue;  
    }

    if (me == 0){
      printf("%3ld: ",start_row);
      print_row(stdout,start_row,k);

      fflush(stdout);
    }

    int s = 2;
    //int initial_rows = 1;
    t_total = 0.0;

    MapReduce *mr = new MapReduce(MPI_COMM_WORLD);
    MR_set_opt(mr);
    
    // Generate the initial table.
    uint64_t curr_count = mr -> map(1,&generate_init_usps,&start_row);
    

    if (me == 0){
      printf(" %d->%12ld; ",1,1L);
    }

    // ---------------- Main Loop ---------------------
    
    while(curr_count != 0){
      MPI_Barrier(MPI_COMM_WORLD);
      t_start = MPI_Wtime();
      
      if (s <= 10 || greedy) 
	curr_count = do_naive(&mr,s);
      else 
	curr_count = do_lookup(&mr,s);
      
      MPI_Barrier(MPI_COMM_WORLD);
      t_stop = MPI_Wtime();
      
      if (me == 0 && curr_count != 0){
	printf("%d->%12ld; ", s, curr_count);
	// printf("Found %ld (%d,%d) strong USPs in %g mins.\n",
	//        curr_count,s,k,
	//        (t_stop - t_start)/60.0);
	fflush(stdout);
      }

      if ((s >= 14 && k == 6) || (s >= 21 && k == 7))
	mr -> print(-1,&fprintUKV);
      
      t_total += t_stop - t_start;
      s++;
    }
    s-=2;

    // ----------------- Clean up --------------------
    if (s > max_s)
      max_s = s;
    
    if (me == 0)
      printf("\t%0.5f mins. Stats: %0.5f hrs; %d; %d\n",
	     t_total / 60.0, (MPI_Wtime()-t_global)/3600.0,s,max_s);
    
    // clean up
    
    delete mr;
  }
  
  MPI_Finalize();
}


