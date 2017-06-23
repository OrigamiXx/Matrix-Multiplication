#include "typedefs.h"
#include "string.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;

#define MAXLINE 1024

/* ----------------------------------------------------------------------
   read_edge
   read edges from file, formatted with 2 vertices per line
   output: key = Vi Vj, value = NULL
------------------------------------------------------------------------- */

void read_edge(int itask, char *file, KeyValue *kv, void *ptr)
{
  char line[MAXLINE];
  EDGE edge;

  FILE *fp = fopen(file,"r");
  while (fgets(line,MAXLINE,fp)) {
    sscanf(line,"%lu %lu",&edge.vi,&edge.vj);
    kv->add((char *) &edge,sizeof(EDGE),NULL,0);
  }
  fclose(fp);
}
