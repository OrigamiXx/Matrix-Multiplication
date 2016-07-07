#include "typedefs.h"
#include "string.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;

#define MAXLINE 1024

/* ----------------------------------------------------------------------
   read_vertex_weight
   read vertices and weights from file
   file format = vertex and floating point weight per line
   output: key = Vi, value = weight
------------------------------------------------------------------------- */

void read_vertex_weight(int itask, char *file, KeyValue *kv, void *ptr)
{
  char line[MAXLINE];
  VERTEX v;
  WEIGHT weight;

  FILE *fp = fopen(file,"r");
  while (fgets(line,MAXLINE,fp)) {
    sscanf(line,"%lu %lg",&v,&weight);
    kv->add((char *) &v,sizeof(VERTEX),(char *) &weight,sizeof(WEIGHT));
  }
  fclose(fp);
}
