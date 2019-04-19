#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "checker.h"
#include <math.h>
#include "constants.h"
#include <map>
#include "assert.h"

int main(int argc, char * argv[]){
  long index = 0;
  int givenR = 3;
  int givenC = 3;
  int i, j;
  FILE * data_file = fopen("data.csv", "w+");
  assert(data_file != NULL);

  fprintf(data_file, "%s", "Row1,Row2,Row3,isSUSP,#of1s,#of2s,#of3s\n");

  for(; index < pow(3, givenR * givenC) -1; index+=1){
    puzzle * p;
    p = create_puzzle_from_index(givenR, givenC, index);

    int count1s = 0;
    int count2s = 0;
    int count3s = 0;

    int result = check(p);
    //int resultR = check_usp_recursive(p);
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
    fprintf(data_file, "%s%s", isUSB, ",");
    fprintf(data_file, "%d%s", count1s, ",");
    fprintf(data_file, "%d%s", count2s, ",");
    fprintf(data_file, "%d", count3s);

    fprintf(data_file, "\n");
    destroy_puzzle(p);
  }

  fclose(data_file);

  //check_all_usp(4, 4, 1);
}
