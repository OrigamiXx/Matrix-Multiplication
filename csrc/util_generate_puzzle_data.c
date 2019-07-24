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
    if(canon)
    {
      canonize_puzzle(p);
    }

    int result = check(p);
    char isUSB[10];

    if(result){ //&& resultR){
        strncpy(isUSB, "1", sizeof(isUSB));
    }
    else{
        strncpy(isUSB, "0", sizeof(isUSB));
    }

    for(int i = 0; i < p -> s; i++){
      for(int j = 0; j < p -> k; j++){
          int entry = get_entry(p, i, j);
          fprintf(data_file, "%d", entry);
      }
      fprintf(data_file, ",");
    }
    int count1s = 0;
    int count2s = 0;
    int count3s = 0;

    for(int i = 0; i < p -> s; i++){
      char row_str[(p-> k) +1];
      int x = get_entry(p, i, 0);
      int y = -1;
      for(int j = 0; j < p -> k; j++){
          int entry = get_entry(p, i, j);
          switch (entry)
          {
            case 1: count1s++;
              break;
            case 2: count2s++;
              break;
            case 3: count3s++;
              break;
          }
          if(entry == x)
          {
            row_str[j] = 'X';
          }
          else if(y == -1 || entry == y)
          {
            y = entry;
            row_str[j] = 'Y';
          }
          else
          {
            row_str[j] = 'Z';
          }
      }
      row_str[p->k] = '\0';
    //  fputs(row_str,data_file);
    //  fprintf(data_file, ",");
    }

    for(int i = 0; i < p -> k; i++){
      char column_str[(p-> s) +1];
      int x = get_entry(p, 0, i);
      int y = -1;
      for(int j = 0; j < p -> s; j++){
          int entry = get_entry(p, j, i);
          if(entry == x)
          {
            column_str[j] = 'X';
          }
          else if(y == -1 || entry == y)
          {
            y = entry;
            column_str[j] = 'Y';
          }
          else
          {
            column_str[j] = 'Z';
          }
      }
      column_str[p->s] = '\0';
  //    fputs(column_str,data_file);
  //    fprintf(data_file, ",");
    }

    double amount_of_numbers = count1s + count2s + count3s;

    double first = count1s;
    double second = count2s;
    double third = count3s;
    if(count2s > count1s)
    {
      first = count2s;
      second = count1s;
    }
    if(count3s > first)
    {
      third = second;
      second = first;
      first = count3s;
    }
    else if(count3s > second)
    {
      third = second;
      second = count3s;
    }

    fprintf(data_file, "%.2f%s", first/amount_of_numbers, ",");
    fprintf(data_file, "%.2f%s", second/amount_of_numbers, ",");
    fprintf(data_file, "%.2f%s", third/amount_of_numbers, ",");
    if(heuristics)
    {
      if(canon)
      {
        swap_stored_state();
        reset_isomorphs();
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
        swap_stored_state();
      }
    }

    for(int i = 0; i < p -> s; i++){
      for(int j = 0; j < p -> k; j++){
        switch (get_entry(p, i, j))
        {
          case 1: fprintf(data_file, "0,0,1,");
            break;
          case 2: fprintf(data_file, "0,1,0,");
            break;
          case 3: fprintf(data_file, "1,0,0,");
            break;
        }
      }
    }

    fprintf(data_file, "%s", isUSB);
    fprintf(data_file, "\n");
}

//To use extra arguments must provide puzzle size "R" "C" Ex: 3 4
// Possible additional arguments:
// -c produces canonized puzzles,
// -r 'integer x' produces x number of random puzzles Ex: -r 500
// -h turns off heuristic generation
// -s adds only SUSPs to the dataset
// -n excludes SUSPs from the dataset
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
  bool only_SUSP = false;
  bool only_not_SUSP = false;
  int random_amount = 0;
  if(argc >= 3) //Name plus row and col     Is there a better way to handle all these potential arguments
  {
    givenR = strtol(argv[1], NULL, 10);
    givenC = strtol(argv[2], NULL, 10);
  }
  while((opt = getopt(argc, argv, "cr:hon")) != -1)
  {
    switch(opt)
    {
      case 'c': canon = true;
          break;
      case 'r': random = true;
                random_amount = strtol(optarg, NULL, 10);
          break;
      case 'h': heuristics = false;
          break;
      case 's':  only_SUSP = true;
          break;
      case 'n': only_not_SUSP = true;
    }
  }
  char filename[256];
  if(canon && random && only_SUSP)
  {
    sprintf(filename, "../data/random_canon_SUSPs_r%d_c%d.csv", givenR, givenC);
  }
  else if(canon && random && only_not_SUSP)
  {
    sprintf(filename, "../data/random_canon_NOTSUSPs_r%d_c%d.csv", givenR, givenC);
  }
  else if(canon && random)
  {
    sprintf(filename, "../data/random_canon_r%d_c%d.csv", givenR, givenC);
  }
  else if (random && only_SUSP)
  {
    sprintf(filename, "../data/random_SUSPs_r%d_c%d.csv", givenR, givenC);
  }
  else if (random && only_not_SUSP)
  {
    sprintf(filename, "../data/random_NOTSUSPs_r%d_c%d.csv", givenR, givenC);
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
/*
  for(int i = 1; i <= givenR; i++)    //Sets up Feature names in dataset
  {
    fprintf(data_file, "%s%d%s","Row",i,",");
  }
  for(int i = 1; i <= givenR; i++)    //Sets up Feature names in dataset
  {
    fprintf(data_file, "%s%d%s","XYZRow",i,",");
  }
  for(int i = 1; i <= givenC; i++)    //Sets up Feature names in dataset
  {
    fprintf(data_file, "%s%d%s","XYZColumn",i,",");
  }
  fprintf(data_file, "%s", "%ofMostFrequentNum,%of2ndMostFrequentNum,%ofLeastFrequentNum,");
  if(heuristics)
  {
    for(int k = 0; k < number_of_heuristics; k++)
    {
      fprintf(data_file, "%s,", heuristic_names[k]);
    }
  }
  for(int i = 1; i <= givenR; i++)    //Sets up Feature names in dataset
  {
    for(int j = 1; j <= givenC; j++)    //Sets up Feature names in dataset
    {
      fprintf(data_file, "%s%d-%d:1%s","OneHot",i,j,",");
      fprintf(data_file, "%s%d-%d:2%s","OneHot",i,j,",");
      fprintf(data_file, "%s%d-%d:3%s","OneHot",i,j,",");
    }
  }
  fprintf(data_file, "%s", "isSUSP\n");
*/
  int current_prog = -1;
  double percent_per_bar = 100;
  current_prog = increase_progress(current_prog, 0, 0, percent_per_bar); //Initilizes progress meter
  long interval = 0;
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

  long next_interval = interval;
  int total_prog = 0;
  float percent_adder = 0;

  long total_puzzles_to_add = 0;
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
    if((canon && !have_seen_isomorph(p)) || !canon)
    {
      if(only_SUSP || only_not_SUSP)
      {
        int result = check(p);
        if(result && only_SUSP)
        {
          add_puzzle_to_datafile(p, data_file, canon, heuristics);
        }
        else if(!result && only_not_SUSP)
        {
          add_puzzle_to_datafile(p, data_file, canon, heuristics);
        }
        else
        {
          index-=1;
        }
      }
      else
      {
        add_puzzle_to_datafile(p, data_file, canon, heuristics);
      }
    }
    else if(random)
    {
      index-=1;
    }
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
