#include "typedefs.h"
#include "string.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;

#define MAXLINE 1024

/* ----------------------------------------------------------------------
   read_vertex_label
   read vertices and labels from file
   file format = vertex and integer label per line
   output: key = Vi, value = label
------------------------------------------------------------------------- */

void read_vertex_label(int itask, char *file, KeyValue *kv, void *ptr)
{
  char line[MAXLINE];
  VERTEX v;
  int label;

  FILE *fp = fopen(file,"r");
  while (fgets(line,MAXLINE,fp)) {
    sscanf(line,"%lu %d",&v,&label);
    kv->add((char *) &v,sizeof(VERTEX),(char *) &label,sizeof(int));
  }
  fclose(fp);
}
