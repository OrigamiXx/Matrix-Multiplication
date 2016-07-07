#include "blockmacros.h"
#include "mapreduce.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;

/* ----------------------------------------------------------------------
   count
   count number of values associated with key
   input: KMV with key and one or more values
   output: key = unchanged, value = count
------------------------------------------------------------------------- */

void count(char *key, int keybytes, char *multivalue,
	   int nvalues, int *valuebytes, KeyValue *kv, void *ptr) 
{
  uint64_t nvalues_total;
  CHECK_FOR_BLOCKS(multivalue,valuebytes,nvalues,nvalues_total)
  int count = static_cast<int> (nvalues_total);
  kv->add(key,keybytes,(char *) &count,sizeof(int));
}
