#include "puzzle.h"
#include "checker.h"
#include "check_PiDD.h"

int main(int argc, char * argv[]){

  // Tester code for check_PiDD().
  if (argc != 2){
    fprintf(stderr,"usage: test_PiDD <filename>\n");
    return -1;
  }

  puzzle * p = create_puzzle_from_file(argv[1]); 
  if (p == NULL) {
    fprintf(stderr,"Error: File does not exist or is not properly formated.\n");
    return -1;
  }
  
  print_puzzle(p);
   
  bool usp = (check(p) == IS_USP);
  bool usp_PiDD = (check_PiDD(p) == IS_USP);

  if (usp)
    printf("Is SUSP.\n");
  else
    printf("Not SUSP.\n");
  
  if (usp != usp_PiDD){
    printf("Warning! Disagrees with check(). ");
    if (usp)
      printf("Says a SUSP isn't one.\n");
    else
      printf("Says non-SUSP is one.\n");
  } else {
    printf("Consistent with check().\n");
  }

  destroy_puzzle(p);

  return 0;
}
