#include "keyvalue.h"
#include "typedefs.h"
using namespace MAPREDUCE_NS;

/* ----------------------------------------------------------------------
   add_weight
   add a default floating point weight to each key, key could be vertex or edge
   input: key = anything, value = NULL
   output: key = unchanged, value = 1.0
------------------------------------------------------------------------- */

void add_weight(uint64_t itask, char *key, int keybytes, char *value,
		int valuebytes, KeyValue *kv, void *ptr)
{
  WEIGHT one = 1.0;
  kv->add(key,keybytes,(char *) &one,sizeof(WEIGHT));
}
