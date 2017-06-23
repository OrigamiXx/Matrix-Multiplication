#include "typedefs.h"
#include "string.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;

#define MAXLINE 1024

/* ----------------------------------------------------------------------
   read_edge_label
   read edges and labels from file
   file format = 2 vertices and integer label per line
   output: key = Vi Vj, value = label
------------------------------------------------------------------------- */

void read_edge_label(int itask, char *file, KeyValue *kv, void *ptr)
{
  char line[MAXLINE];
  EDGE edge;
  int label;

  FILE *fp = fopen(file,"r");
  while (fgets(line,MAXLINE,fp)) {
    sscanf(line,"%lu %lu %d",&edge.vi,&edge.vj,&label);
    kv->add((char *) &edge,sizeof(EDGE),(char *) &label,sizeof(int));
  }
  fclose(fp);
}
