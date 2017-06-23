#include "typedefs.h"
#include "stdio.h"

/* ----------------------------------------------------------------------
   print_edge
   print out an edge to a file
   input: key = Vi Vj, value = NULL
------------------------------------------------------------------------- */

void print_edge(char *key, int keybytes,
		char *value, int valuebytes, void *ptr) 
{
  FILE *fp = (FILE *) ptr;
  EDGE *edge = (EDGE *) key;
  fprintf(fp,"%lu %lu\n",edge->vi,edge->vj);
}
