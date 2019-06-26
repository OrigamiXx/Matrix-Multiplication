#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "checker.h"
#include <math.h>
#include "constants.h"
#include <map>
#include "assert.h"

#include "searcher2.h"
#include "canonization.h"

#define DEFAULT_ROW 3
#define DEFAULT_COL 3

int main(int argc, char * argv[]){
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

  for(i = 1; i <= givenR; i++)
  {
    fprintf(data_file, "%s%d%s","Row",i,",");
  }
  fprintf(data_file, "%s", "#of1s,#of2s,#of3s,");

  for(int k = 0; k < number_of_heuristics; k++)
  {
    fprintf(data_file, "%s,", heuristic_names[k]);
  }
  fprintf(data_file, "%s", "isSUSP\n");

  if(cano)
  {
    reset_isomorphs();
  }

  for(; index < pow(3, givenR * givenC); index+=1){
    puzzle * p;
    p = create_puzzle_from_index(givenR, givenC, index);

    if((cano && !have_seen_isomorph(p)) || !cano)
    {
      int count1s = 0;
      int count2s = 0;
      int count3s = 0;

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

      printf("TEST%d\n", get_num_isomorphs());

      store_state();

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
      }

      revert_stored_state();

      fprintf(data_file, "%s%s", isUSB, ",");
      fprintf(data_file, "\n");
      destroy_puzzle(p);

    }
  }

  fclose(data_file);
  printf("DONE\n");

}
