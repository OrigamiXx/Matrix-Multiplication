#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "checker.h"
#include <math.h>
#include "constants.h"
#include <map>
#include "assert.h"
#include <unistd.h>
#include "searcher2.h"
#include "canonization.h"
#include <time.h>

#define DEFAULT_ROW 3
#define DEFAULT_COL 3
#define NUM_INTERVALS 50 //Creates number of intervals in progress meter

void print_progress(size_t count, double eta)
{
	const char prefix[] = "Progress: [";
	const char suffix[] = "]";
	const size_t prefix_length = sizeof(prefix) - 1;
	const size_t suffix_length = sizeof(suffix) - 1;
	char *buffer = (char*)calloc(NUM_INTERVALS + prefix_length + suffix_length + 1, 1);

	strcpy(buffer, prefix);
  size_t i = 0;
	for (; i < NUM_INTERVALS; ++i)
	{
		buffer[prefix_length + i] = i < count ? '#' : ' ';
	}
	strcpy(&buffer[prefix_length + i], suffix);
  float percent = 100*(count/(float)NUM_INTERVALS);

	printf("\b%c[2K\r%s  %.0f%%  ETA: %.2f seconds", 27, buffer, percent, eta);
	fflush(stdout);
	free(buffer);
}

int increase_progress(int current_progress, double eta)
{
  if(current_progress < NUM_INTERVALS)
  {
    print_progress(current_progress+1, eta);
  }
	return current_progress+1;
}

int main(int argc, char * argv[]){

  double total_time;
	clock_t start, end;
  start = clock(); //Begins tracking program runtime

  long index = 0;
  int givenR = 3;
  int givenC = 3;
  bool cano = false;
  if(argc == 3) //Name plus row and col
  {
    givenR = strtol(argv[1], NULL, 10);
    givenC = strtol(argv[2], NULL, 10);
  }
  else if(argc == 4)
  {
    givenR = strtol(argv[1], NULL, 10);
    givenC = strtol(argv[2], NULL, 10);
    if(!strcmp(argv[3],"-c"))
    {
      cano = true;
    }
  }
  int i, j;

  char filename[256];

  if(cano)
  {
    sprintf(filename, "../data/cano_r%d_c%d.csv", givenR, givenC);
  }
  else
  {
    sprintf(filename, "../data/r%d_c%d.csv", givenR, givenC);
  }
  FILE * data_file = fopen(filename, "w+");
  assert(data_file != NULL);

  for(i = 1; i <= givenR; i++)    //Sets up Feature names in dataset
  {
    fprintf(data_file, "%s%d%s","Row",i,",");
  }
  fprintf(data_file, "%s", "#of1s,#of2s,#of3s,");

  for(int k = 0; k < number_of_heuristics; k++)
  {
    fprintf(data_file, "%s,", heuristic_names[k]);
  }
  fprintf(data_file, "%s", "isSUSP\n");

  int current_prog = -1;
  current_prog = increase_progress(current_prog, 0); //Initilizes progress meter
  int interval = pow(3, givenR * givenC)/ NUM_INTERVALS;
  int next_interval = interval;

  for(; index < pow(3, givenR * givenC); index+=1){

    if(index == next_interval)
    {
      clock_t temp = clock();
      double time_elapsed = ((double) (temp - start)) / CLOCKS_PER_SEC;
      float eta = (NUM_INTERVALS*(time_elapsed/(current_prog+1))) - time_elapsed;
      current_prog = increase_progress(current_prog, eta);
      next_interval += interval;
    }

    puzzle * p;
    p = create_puzzle_from_index(givenR, givenC, index);

    if((cano && !have_seen_isomorph(p)) || !cano)
    {
      int count1s = 0;
      int count2s = 0;
      int count3s = 0;

      if(cano)
      {
        canonize_puzzle(p);
      }

      int result = check(p);
      char isUSB[10];

      if(result){ //&& resultR){
          strncpy(isUSB, "True", sizeof(isUSB));
      }
      else{
          strncpy(isUSB, "False", sizeof(isUSB));
      }
      for(i = 0; i < p -> s; i++){
        for(j = 0; j < p -> k; j++){
  	        fprintf(data_file, "%d", get_entry(p, i, j));
          	switch (get_entry(p, i, j))
          	{
          	  case 1: count1s++;
          	    break;
          	  case 2: count2s++;
          	    break;
          	  case 3: count3s++;
          	    break;
          	}
        }
        fprintf(data_file, ",");
      }
      fprintf(data_file, "%d%s", count1s, ",");
      fprintf(data_file, "%d%s", count2s, ",");
      fprintf(data_file, "%d%s", count3s, ",");

      if(cano)
      {
        store_state();
      }
      for(int k = 0; k < number_of_heuristics; k++)
      {
        ExtensionGraph * eg = new ExtensionGraph(p);
        priority_queue<heuristic_result> * hrq;

        search_heuristic_t h = heuristic_functions[k];

        hrq = (*h)(p, eg);

        if(!hrq -> empty())
        {
          fprintf(data_file, "%d%s", (hrq -> top()).ideal, ",");
        }
        else
        {
          fprintf(data_file, "%d%s", 0, ",");
        }
        reset_isomorphs();

        delete(eg);
        delete(hrq);
      }
      if(cano)
      {
      revert_stored_state();
      }
      fprintf(data_file, "%s%s", isUSB, ",");
      fprintf(data_file, "\n");
    }
      destroy_puzzle(p);
  }
  fclose(data_file);

  end = clock();
	total_time = ((double) (end - start)) / CLOCKS_PER_SEC;
  increase_progress(NUM_INTERVALS-1, 0); //Ensures progress bar is full once program ends
	printf("\nProgram Runtime: %.2f seconds\n", total_time);
}
