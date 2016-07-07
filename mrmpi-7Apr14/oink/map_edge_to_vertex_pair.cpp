#include "typedefs.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;

/* ----------------------------------------------------------------------
   edge_to_vertex_pair
   emit 1 vertex for each edge, just first one
   input: key = Vi Vj, value = NULL
   output: key = Vi, value = NULL
------------------------------------------------------------------------- */

void edge_to_vertex_pair(uint64_t itask, char *key, int keybytes, char *value,
			 int valuebytes, KeyValue *kv, void *ptr)
{
  EDGE *edge = (EDGE *) key;
  kv->add((char *) &edge->vi,sizeof(VERTEX),(char *) &edge->vj,sizeof(VERTEX));
}
