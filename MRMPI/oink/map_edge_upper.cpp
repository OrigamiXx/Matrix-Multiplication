#include "typedefs.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;

/* ----------------------------------------------------------------------
   edge_upper
   emit each edge with Vi < Vj, drop self-edges with Vi = Vj
   input: key = Vi Vj, value = NULL
   output: key = Vi Vj, value = NULL, with Vi < Vj
------------------------------------------------------------------------- */

void edge_upper(uint64_t itask, char *key, int keybytes, char *value,
		int valuebytes, KeyValue *kv, void *ptr)
{
  EDGE *edge = (EDGE *) key;
  if (edge->vi < edge->vj) kv->add(key,keybytes,NULL,0);
  else if (edge->vi > edge->vj) {
    EDGE newedge;
    newedge.vi = edge->vj;
    newedge.vj = edge->vi;
    kv->add((char *) &newedge,sizeof(EDGE),NULL,0);
  }
}
