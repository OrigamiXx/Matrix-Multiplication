#include "typedefs.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;

/* ----------------------------------------------------------------------
   edge_to_vertices
   emit 2 vertices for each edge
   input: key = Vi Vj, value = NULL
   output:
     key = Vi, value = NULL
     key = Vj, value = NULL
------------------------------------------------------------------------- */

void edge_to_vertices(uint64_t itask, char *key, int keybytes, char *value,
		      int valuebytes, KeyValue *kv, void *ptr)
{
  EDGE *edge = (EDGE *) key;
  kv->add((char *) &edge->vi,sizeof(VERTEX),NULL,0);
  kv->add((char *) &edge->vj,sizeof(VERTEX),NULL,0);
}
