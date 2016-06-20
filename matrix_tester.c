#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include "permutation.h"
#include "groups.h"
#include "matrix.h"
#include "CheckUSP.h"
#include <time.h>


void main(int argc, char * argv[]) {

  puzzle * p = create_puzzle_from_file("puzzles/usp_4_4.puz");

  if (is_usp(p))
    printf("is_usp says p is a USP.\n");
  else
    printf("is_usp says p is NOT a USP.\n");

  if (CheckUSP(p))
    printf("CheckUSP says p is a USP.\n");
  else 
    printf("CheckUSP says p is NOT a USP.\n");

}
