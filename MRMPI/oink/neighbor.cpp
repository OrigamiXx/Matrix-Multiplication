/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "typedefs.h"
#include "string.h"
#include "stdlib.h"
#include "neighbor.h"
#include "object.h"
#include "style_map.h"
#include "error.h"

#include "blockmacros.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

/* ---------------------------------------------------------------------- */

Neighbor::Neighbor(OINK *oink) : Command(oink)
{
  ninputs = 1;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void Neighbor::run()
{
  int me;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  // MRe = Eij : NULL

  MapReduce *mre = obj->input(1,read_edge,NULL);
  MapReduce *mrn = obj->create_mr();

  mrn->map(mre,map1,NULL);
  mrn->collate(NULL);
  mrn->reduce(reduce1,NULL);

  obj->output(1,mrn,print,NULL);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void Neighbor::params(int narg, char **arg)
{
  if (narg != 0) error->all("Illegal sgi_prune command");
}

/* ---------------------------------------------------------------------- */

void Neighbor::print(char *key, int keybytes, char *value, int valuebytes,
		     void *ptr) 
{
  VERTEX vi,vj;

  FILE *fp = (FILE *) ptr;
  vi = *(VERTEX *) key;
  fprintf(fp,"%lu",vi);

  int n = valuebytes/sizeof(VERTEX);
  int offset = 0;
  for (int i = 0; i < n; i++) {
    vj = *(VERTEX *) &value[offset];
    fprintf(fp," %lu",vj);
    offset += sizeof(VERTEX);
  }
  fprintf(fp,"\n");
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

void Neighbor::map1(uint64_t itask, char *key, int keybytes, 
		    char *value, int valuebytes, KeyValue *kv, void *ptr) 
{
  EDGE *edge = (EDGE *) key;
  kv->add((char *) &edge->vi,sizeof(VERTEX),(char *) &edge->vj,sizeof(VERTEX));
  kv->add((char *) &edge->vj,sizeof(VERTEX),(char *) &edge->vi,sizeof(VERTEX));
}

/* ---------------------------------------------------------------------- */

void Neighbor::reduce1(char *key, int keybytes, char *multivalue, int nvalues,
		       int *valuebytes, KeyValue *kv, void *ptr) 
{
  uint64_t nvalues_total;
  CHECK_FOR_BLOCKS(multivalue,valuebytes,nvalues,nvalues_total)

  int n = nvalues_total;
  VERTEX *neighlist = new VERTEX[n];

  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  int offset = 0;
  for (int i = 0; i < nvalues; i++) {
    neighlist[i] = *(VERTEX *) &multivalue[offset];
    offset += valuebytes[i];
  }

  END_BLOCK_LOOP

  kv->add(key,keybytes,(char *) neighlist,n*sizeof(VERTEX));
  delete [] neighlist;
}
