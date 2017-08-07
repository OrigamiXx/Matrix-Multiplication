/* 
 * ???
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <map>

#include "puzzle.h"
#include "constants.h"
#include "timing.h"
#include "usp.h"
#include "3DM_to_SAT.h"
#include "3DM_to_MIP.h"
#include "permutation.h"


int main(int argc, char * argv[]){
  // distribution
  int one = 0, two=0, three=0, four=0;
  int five=0, six=0, seven=0, eight = 0;
  int checked = 0;
  int checked2 = 0;
  double total = 0;
  double individual = 0;
  int usps = 0;
  char buff[256];
  int element;
  int next_element;
  int row;
  int row_index;
  int time = 46800;
  check_t thread_res, mip_res, sat_res, bi_res;
  double thread_time, mip_time, sat_time, bi_time;
  char csv_input[50];
  char usp_num[10];

  struct timespec begin={0,0}, end={0,0};
  FILE * puzzles = fopen("data/13-by-6-big.txt", "r");
  //FILE * puzzles = fopen("1", "r");
  FILE * record = fopen("data/14-by-6-time-record.csv", "w");
  assert(record != NULL);
  fprintf(record, "puzzle_number, thread_check, mip_check, sat_check\n");
  fflush(record);
  while(!feof(puzzles)){
    puzzle * p = create_puzzle(14,6);
    row = 0;
    row_index= 0;
    while (fgets(buff,sizeof(buff),puzzles) != NULL && buff[0]!= '\n'){
      element = buff[p->k-1] - '0';
      element = element -1;
      next_element = buff[p->k-2] - '0';
      next_element = next_element-1;
      row_index = element*3 + next_element;
      for(int i = p->k-2; i>0; i--){
         next_element = buff[i-1] - '0';
         next_element = next_element - 1;
         //printf("%d",element);
         row_index = row_index*3 + next_element;
         //p->puzzle[rows][i] = element;
      }
      p->puzzle[row] = row_index;
      row++;
      //printf("got here \n");
    }
    //print_puzzle(p);
    if (total>time) {
      break;
    }
    //itoa(checked, usp_num, 10);
    for (int i = 0; i < 729; i++){
      p->puzzle[row] = i;
      //fwrite(checked, sizeof(int), 1, record);
      //fwrite(",", sizeof(char), 1, record);
      //print_puzzle(p);
      //printf("got here \n");
      int test_count = 0;
      int total_res = 0;
      mip_res = time_check(&check_MIP, p, &mip_time);
      total_res += mip_res;
      test_count++;
      //ftoa(individual, timing, 5);

      sat_res = time_check(&check_SAT, p, &sat_time);
      total_res += sat_res;
      test_count++;
      //ftoa(individual, timing, 5);

      thread_res = time_check(&check_SAT_MIP, p, &thread_time);
      total_res += thread_res;
      test_count++;

      //bi_res = time_check(&check_usp_bi, p, &bi_time);
      //total_res += bi_res;
      //test_count++;
      
      //print_puzzle(p);
      //if (check(p->puzzle,p->s,p->k)){
        //(check_usp_bi(p->puzzle,p->s,p->k)){
        //(check_MIP(p)){
        //(check_SAT(p)){
        //printf("this puzzle is a new 14 by 6y usp it's rua %d and index %d\n", checked+1, i);
        //write_puzzle(p, -1);
        //print_puzzle(p);
        //usps++;
      //}
      checked2++;

      //ftoa(individual, timing, 5);
      //printf("%d,%.5f,%.5f,%.5f\n", checked, thread_time, mip_time, sat_time);
      //sprintf(csv_input, "%d,%.5f,%.5f,%.5f\n", checked, thread_time, mip_time, sat_time);
      fprintf(record, "%d,%.5f,%.5f,%.5f\n", checked, thread_time, mip_time, sat_time);
      fflush(record);
      // fwrite(individual, sizeof(double), 1, record);
      // fwrite("\n", sizeof(char), 1, record);

      
      if (total_res != test_count && total_res != 0){ //mip_res != sat_res || sat_res != thread_res){
        printf("Conflict, mip = %d, sat = %d, thread = %d", mip_res, sat_res, thread_res);
        return -1;
      }

      if(thread_res == 1){
        printf("this puzzle is a new 14 by 6y usp it's rua %d and index %d\n", checked+1, i);
      }
      //total += individual;
      //printf("this one %d takes %.5f\n", i, ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec));
      if (individual>=0 && individual < 0.01){
        one++;
      } else if(individual >= 0.01 && individual < 0.1){
        two++;
      } else if (individual >= 0.1 && individual < 1){
        three++;
      } else if (individual >= 1 && individual < 10){
        four++;
      } else if (individual >= 10 && individual < 50){
        five++;
      } else if (individual >= 50 && individual < 100){
        six++;
      } else if (individual >= 100 && individual < 300){
        seven++;
      } else {
        eight++;
      }
      if (total>time) {
        break;
      }
    }
    destroy_puzzle(p);
    checked++;
    //printf("rua%d\n",checked);

  }
  fclose(record);
  finalize_check_MIP();
  printf("checked 13by6 %d\n", checked);
  printf("checked 14by6 %d\n", checked2);
  printf("found %d\n", usps);
  printf("total time %.5f and avg checking time %.5f\n",total, total/checked2);
  printf("there are: \n");
  printf("%d of 0-0.01 \n", (int) one);
  printf("%d of 0.01-0.1 \n", (int) two);
  printf("%d of 0.1-1 \n", three);
  printf("%d of 1-10 \n", four);
  printf("%d of 10-50 \n", five);
  printf("%d of 50-100 \n", six);
  printf("%d of 100-300 \n", seven);
  printf("%d of 300+ \n", eight);







  //if(check_usp_recursive(p)){
  //int i = check_usp(p);
  //printf("hello%d\n", i);
  //}


  //printf("result = %d\n",check_usp(p));
  //printf("%d\n",sizeof(int));
  //printf("%d\n",sizeof(long));
  //printf("%d\n",sizeof(long long));




    //int givenR = 8;
    //int givenC = 6;

   //int * puzzle1 = (int *) malloc(sizeof(int *)*givenR);
  //puzzle1[0] = 279;
  //puzzle1[1]= 284;
  //puzzle1[2]= 290;
  //puzzle1[3]= 297;
  //puzzle1[4] = 318;
  //puzzle1[5] = 350;
  //puzzle1[6] = 389;
  //puzzle1[7] = 487;
  //puzzle1[8] = 519;
  //puzzle1[9] = 586;
  //puzzle1[10] = 591;
  //puzzle1[11] = 630;
  //puzzle1[12] = 637;
  //puzzle1[13] = 642;



  //puzzle * result = (puzzle *) (malloc(sizeof(puzzle)));
  //result->s = givenR;
  //result->k = givenC;
  //result -> puzzle = puzzle1;

  //print_puzzle(result);

  //topcheck_usp(result);
  //printf("%d", check_usp(result));

  //  printf("%d", check(puzzle1, givenR, givenC));

  /* puzzle * sample; */
  /* 	sample = create_puzzle_from_index(8, 6, 10); */

  /* 	print_puzzle(sample); */
  /* 	printf("%d", check_usp_recursive(sample)); */
  /* 	write_puzzle(sample, 10); */
  /* 	destroy_puzzle(sample); */

  return 0;

}
