#include "keyvalue.h"
using namespace MAPREDUCE_NS;

/* ----------------------------------------------------------------------
   add_label
   add a default integer label to each key, key could be vertex or edge
   input: key = anything, value = NULL
   output: key = unchanged, value = 1
------------------------------------------------------------------------- */

void add_label(uint64_t itask, char *key, int keybytes, char *value,
	       int valuebytes, KeyValue *kv, void *ptr)
{
  int one = 1;
  kv->add(key,keybytes,(char *) &one,sizeof(int));
}
