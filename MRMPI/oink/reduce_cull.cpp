#include "mapreduce.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;

/* ----------------------------------------------------------------------
   cull
   eliminate duplicate values
   input: KMV with key and one or more values (assumed to be duplicates)
   output: key = unchanged, value = first value
------------------------------------------------------------------------- */

void cull(char *key, int keybytes, char *multivalue,
	  int nvalues, int *valuebytes, KeyValue *kv, void *ptr) 
{
  if (!multivalue) {
    MapReduce *mr = (MapReduce *) valuebytes;
    mr->multivalue_block(0,&multivalue,&valuebytes);
  }
  kv->add(key,keybytes,multivalue,valuebytes[0]);
}
