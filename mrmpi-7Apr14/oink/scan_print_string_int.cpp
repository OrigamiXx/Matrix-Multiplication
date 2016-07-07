#include "stdio.h"

/* ----------------------------------------------------------------------
   print_string_int
   print out key as string and value as int, to a file
   input: key = string, value = int
------------------------------------------------------------------------- */

void print_string_int(char *key, int keybytes, 
		      char *value, int valuebytes, void *ptr) 
{
  FILE *fp = (FILE *) ptr;
  int count = *(int *) value;
  fprintf(fp,"%s %d\n",key,count);
}
