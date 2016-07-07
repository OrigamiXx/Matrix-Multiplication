#include "typedefs.h"
#include "stdio.h"

/* ----------------------------------------------------------------------
   print_vertex
   print out an vertex to a file
   input: key = Vi, value = NULL
------------------------------------------------------------------------- */

void print_vertex(char *key, int keybytes,
		  char *value, int valuebytes, void *ptr) 
{
  FILE *fp = (FILE *) ptr;
  VERTEX v = *(VERTEX *) key;
  fprintf(fp,"%lu\n",v);
}
