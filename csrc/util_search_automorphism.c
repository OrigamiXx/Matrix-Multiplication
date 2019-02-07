// Simple tester for the puzzle module.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "checker.h"
#include "constants.h"
#include "puzzle.h"
#include "canonization.h"
#include "nauty.h"

void verify_isomorphs(const char * s1, const char * s2, bool isomorphic){

  reset_isomorphs();
  
  puzzle * p1 = create_puzzle_from_string((char *)s1);
  puzzle * p2 = create_puzzle_from_string((char *)s2);

  assert(!have_seen_isomorph(p1,true));
  assert((isomorphic ? have_seen_isomorph(p2,true) : !have_seen_isomorph(p2,true)));
  canonize_puzzle(p1);
  canonize_puzzle(p2);
  assert(have_seen_isomorph(p1,true));
  assert(have_seen_isomorph(p2,true));
  
  destroy_puzzle(p1);
  destroy_puzzle(p2);

}

int main(int argc, char * argv[]){

  srand48(time(NULL));
  
  reset_isomorphs();


  int s = 4;
  int k = 5;
  puzzle * p = create_puzzle(s, k);

  unsigned long n = 10000; // 5, 22, 1006
  unsigned long m = SETWORDSNEEDED(n);
    
  nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
  
  graph * g = new graph[m * n];
  EMPTYGRAPH(g,m,n);
  unsigned long num0 = 1;
  int part0[1] = {0};
  unsigned long num1 = 0;
  int part1[n];
  unsigned long num2 = 0;
  int part2[n];
  unsigned long num3 = 0;
  int part3[n];
  unsigned long num4 = 0;
  int part4[n];

  char filename[100];
  sprintf(filename, "search_graph_k_%d.csv", k);
  FILE * outfile = fopen(filename,"w");
  
  for (puzzle_row r1 = 0; r1 < p -> max_row; r1++){
    
    p -> puzzle[0] = r1;
    int puzzle_index1 = 0;    
    p -> s = 1;
    bool res1 = have_seen_isomorph(p, true, &puzzle_index1);

    ADDONEEDGE(g, 0, puzzle_index1, m);
    fprintf(outfile, "%d;%d\n", 0, puzzle_index1);

    if (res1)
      continue;
    else {
      part1[num1] = puzzle_index1;
      num1++;
    }

    p -> s = 2;

    for (puzzle_row r2 = r1+1; r2 < p -> max_row; r2++){
      
      p -> puzzle[1] = r2;
      int puzzle_index2 = 0;
      p -> s = 2;
      if (NOT_USP == check(p)) continue;
      bool res2 = have_seen_isomorph(p, true, &puzzle_index2);

      // Add edge 
      ADDONEEDGE(g, puzzle_index1, puzzle_index2, m);
      fprintf(outfile, "%d;%d\n", puzzle_index1, puzzle_index2);

      if (res2)
	continue;
      else {
	part2[num2] = puzzle_index2;
	num2++;
      }
      

      puzzle * p2 = create_puzzle_copy(p);
      canonize_puzzle(p2);
      print_puzzle(p2);
      destroy_puzzle(p2);
      printf("----\n");
      
      p -> s = 3;

      for (puzzle_row r3 = r2+1; r3 < p -> max_row; r3++){
	//printf("\r%14.8f%%", (float)(r1 * p -> max_row * p -> max_row + r2 * p -> max_row + r3) / (p -> max_row * p -> max_row * p -> max_row) * 100.0);
	//fflush(stdout);

	p -> puzzle[2] = r3;
	int puzzle_index3 = 0;
	
	p -> s = 3;
	if (NOT_USP == check(p)) continue;
	bool res3 = have_seen_isomorph(p, true, &puzzle_index3);

	// Add edge 
	ADDONEEDGE(g, puzzle_index2, puzzle_index3, m);
	fprintf(outfile, "%d;%d\n", puzzle_index2, puzzle_index3);
	
	if (res3)
	  continue;
	else {
	  part3[num3] = puzzle_index3;
	  num3++;
	}
	
	
	for (puzzle_row r4 = r3+1; r4 < p -> max_row; r4++){
	  //printf("\r%14.8f%%", (float)(r1 * p -> max_row * p -> max_row + r2 * p -> max_row + r3) / (p -> max_row * p -> max_row * p -> max_row) * 100.0);
	  //fflush(stdout);
	  
	  p -> puzzle[3] = r4;
	  int puzzle_index4 = 0;
	  
	  p -> s = 4;
	  if (NOT_USP == check(p)) continue;
	  bool res4 = have_seen_isomorph(p, true, &puzzle_index4);
	  
	  // Add edge 
	  ADDONEEDGE(g, puzzle_index3, puzzle_index4, m);
	  fprintf(outfile, "%d;%d\n", puzzle_index3, puzzle_index4);
	  
	  if (res4)
	    continue;
	  else {
	    part4[num4] = puzzle_index4;
	    num4++;
	  }

	  assert(num0 + num1 + num2 + num3 + num4 <= n);
	
	}
      }
    }
  }


  int lab[n];
  int ptn[n];
  int orbits[n];
  static DEFAULTOPTIONS_GRAPH(options);
  options.writeautoms = FALSE;
  options.defaultptn = FALSE;
  statsblk stats;
  // Depth 0.
  lab[0] = 0;
  ptn[0] = 0;

  int prev = 1;
  for (int i = 0; i < num1; i++){
    lab[i + prev] = part1[i];
    if (i == num1 - 1)
      ptn[i + prev] = 0;
    else
      ptn[i + prev] = 1;
  }
  prev += num1;

  for (int i = 0; i < num2; i++){
    lab[i + prev] = part2[i];
    if (i == num2 - 1)
      ptn[i + prev] = 0;
    else
      ptn[i + prev] = 1;
  }
  prev += num2;

  for (int i = 0; i < num3; i++){
    lab[i + prev] = part3[i];
    if (i == num3 - 1)
      ptn[i + prev] = 0;
    else
      ptn[i + prev] = 1;
  }
  prev += num3;

  for (int i = 0; i < num4; i++){
    lab[i + prev] = part4[i];
    if (i == num4 - 1)
      ptn[i + prev] = 0;
    else
      ptn[i + prev] = 1;
  }
  prev += num4;

  assert(prev <= n);


  printf("Labels: ");
  for (int i = 0; i < prev; i++){
    printf("%d ", lab[i]);
  }
  printf("\n");

  printf("Parts: ");
  for (int i = 0; i < prev; i++){
    printf("%d ", ptn[i]);
  }
  printf("\n");

  printf("Running nauty %d\n", prev);
  densenauty(g,lab,ptn,orbits,&options,&stats,m,n,NULL);

  bzero(lab, prev * sizeof(int));
  printf("Orbits: ");
  for (int i = 0; i < prev; i++){
    printf("%d ", orbits[i]);
    lab[orbits[i]] = 1;
  }
  printf("\n");

  int num_orbits = 0;
  for (int i = 0; i < prev; i++){
    if (lab[i] == 1)
      num_orbits++;
  }
  printf("Num orbits: %d / %d\n", num_orbits, prev);
  
  
  
  //printf("\r  100.00000000%%\n");
    printf("Num distinct puzzles: 1: %ld, 2: %ld, 3: %ld, 4: %ld\n", num1, num2, num3, num4);
  fclose(outfile);
  
  return 0;
}


