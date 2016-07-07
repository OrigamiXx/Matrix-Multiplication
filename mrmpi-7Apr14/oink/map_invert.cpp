#include "keyvalue.h"
using namespace MAPREDUCE_NS;

/* ----------------------------------------------------------------------
   invert
   invert key and value
   input: key, value
   output: key = value, value = key
------------------------------------------------------------------------- */

void invert(uint64_t itask, char *key, int keybytes, char *value,
	    int valuebytes, KeyValue *kv, void *ptr)
{
  kv->add(value,valuebytes,key,keybytes);
}
