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
#define NUM_INTERVALS 50 //Sets number of symbols in progress meter

//Converts seconds into Hours, Minutes, Seconds notation and prints it.
void convert_seconds_to_time(int seconds)
{
    int hour = seconds / 3600;
    seconds %= 3600;
    int minutes = seconds / 60 ;
    seconds %= 60;
    int second = seconds;

    printf("%d H, %d M, %d S", hour, minutes, second);
}
//Adds 1 symbol to progress bar. Can not overflow.
int increase_progress(size_t current_progress, double eta, float percent_adder, int percent_per_bar)
{
  current_progress+=1;
  if(current_progress <= NUM_INTERVALS)
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
  		buffer[prefix_length + i] = i < current_progress ? '#' : ' ';
  	}
  	strcpy(&buffer[prefix_length + i], suffix);
    float percent = percent_adder;

  	printf("\b%c[2K\r%s  %.0f%%  ETA: ", 27, buffer, percent, eta);
    convert_seconds_to_time(eta);
  	fflush(stdout);
  	free(buffer);
  }
  return current_progress;
}

//Given Index and Puzzle Size, creates puzzle and features and adds them to the data file.
void add_puzzle_to_datafile(puzzle * p, FILE * data_file, bool canon, bool heuristics)
{
  if((canon && !have_seen_isomorph(p)) || !canon)
  {
    int count1s = 0;
    int count2s = 0;
    int count3s = 0;

    if(canon)
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
    for(int i = 0; i < p -> s; i++){
      for(int j = 0; j < p -> k; j++){
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
    if(heuristics)
    {
      if(canon)
      {
        store_state();
      }
      for(int k = 0; k < number_of_heuristics; k++)
      {
        ExtensionGraph * eg = new ExtensionGraph(p); //Possibly move outside loop
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
      if(canon)
      {
      revert_stored_state();
      }
    }

    fprintf(data_file, "%s%s", isUSB, ",");
    fprintf(data_file, "\n");
  }
}

//To use extra arguments must provide puzzle size "R" "C" Ex: 3 4
// Possible additional arguments:
// -c produces canonized puzzles,
// -r 'integer x' produces x number of random puzzles Ex: -r 500
// -h turns off heuristics
int main(int argc, char * argv[]){

  double total_time;
	clock_t start, end;
  start = clock(); //Begins tracking program runtime

  int opt;
  int givenR = 3;
  int givenC = 3;
  bool canon = false;
  bool random = false;
  bool heuristics = true;
  int random_amount = 0;
  if(argc >= 3) //Name plus row and col     Is there a better way to handle all these potential arguments
  {
    givenR = strtol(argv[1], NULL, 10);
    givenC = strtol(argv[2], NULL, 10);
  }
  while((opt = getopt(argc, argv, "cr:h")) != -1)
  {
    switch(opt)
    {
      case 'c': canon = true;
          break;
      case 'r': random = true;
                random_amount = strtol(optarg, NULL, 10) -1;
          break;
      case 'h': heuristics = false;
          break;
    }
  }
  char filename[256];
  if(canon && random)
  {
    sprintf(filename, "../data/random_canon_r%d_c%d.csv", givenR, givenC);
  }
  else if(canon)
  {
    sprintf(filename, "../data/canon_r%d_c%d.csv", givenR, givenC);
  }
  else if(random)
  {
    sprintf(filename, "../data/random_r%d_c%d.csv", givenR, givenC);
  }
  else
  {
    sprintf(filename, "../data/r%d_c%d.csv", givenR, givenC);
  }
  FILE * data_file = fopen(filename, "w+");
  assert(data_file != NULL);

  for(int i = 1; i <= givenR; i++)    //Sets up Feature names in dataset
  {
    fprintf(data_file, "%s%d%s","Row",i,",");
  }
  fprintf(data_file, "%s", "#of1s,#of2s,#of3s,");
  if(heuristics)
  {
    for(int k = 0; k < number_of_heuristics; k++)
    {
      fprintf(data_file, "%s,", heuristic_names[k]);
    }
  }
  fprintf(data_file, "%s", "isSUSP\n");

  int current_prog = -1;
  double percent_per_bar = 100;
  current_prog = increase_progress(current_prog, 0, 0, percent_per_bar); //Initilizes progress meter
  int interval = 0;
  if(random)
  {
    interval = random_amount/(NUM_INTERVALS*(100/percent_per_bar));
  }
  else
  {
    interval = ceil(pow(3, givenR * givenC)/ (NUM_INTERVALS*(100/percent_per_bar)));
  }
  while(interval > 8000 && percent_per_bar > 1) //Sets up how many progress bars to run, based on length of program
  {
    percent_per_bar  = percent_per_bar/10;
    if(random)
    {
      interval = random_amount/(NUM_INTERVALS*(100/percent_per_bar));
    }
    else
    {
      interval = ceil(pow(3, givenR * givenC)/ (NUM_INTERVALS*(100/percent_per_bar)));
    }
  }

  int next_interval = interval;
  int total_prog = 0;
  float percent_adder = 0;

  int total_puzzles_to_add = 0;
  if(random)
  {
    total_puzzles_to_add = random_amount;
  }
  else
  {
    total_puzzles_to_add = pow(3, givenR * givenC);
  }

  for(long index = 0; index < total_puzzles_to_add; index+=1)
  {
    if(index == next_interval)
    {
      clock_t temp = clock();
      total_prog += 1;
      double time_elapsed = ((double) (temp - start)) / CLOCKS_PER_SEC;
      float eta = ((NUM_INTERVALS*(100/percent_per_bar))*(time_elapsed/(total_prog))) - time_elapsed;
      if(current_prog == NUM_INTERVALS-1)
      {
        current_prog = -1;
        percent_adder += percent_per_bar;
      }
      current_prog = increase_progress(current_prog, eta, percent_adder, percent_per_bar);
      next_interval += interval;
    }

    puzzle * p;
    if(random)
    {
      p = create_puzzle(givenR, givenC);
      randomize_puzzle(p);
    }
    else
    {
      p = create_puzzle_from_index(givenR, givenC, index);
    }
    add_puzzle_to_datafile(p, data_file, canon, heuristics);
    destroy_puzzle(p);
  }

  fclose(data_file);

  end = clock();
	total_time = ((double) (end - start)) / CLOCKS_PER_SEC;
  increase_progress(NUM_INTERVALS-1, 0, 100, 0); //Ensures progress bar is full once program ends
	printf("\nProgram Runtime: ");
  convert_seconds_to_time(total_time);
  printf("\n");
}
