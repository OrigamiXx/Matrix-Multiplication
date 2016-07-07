/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "typedefs.h"
#include "string.h"
#include "stdlib.h"
#include "cc_stats.h"
#include "object.h"
#include "style_map.h"
#include "style_reduce.h"
#include "error.h"

#include "blockmacros.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

#define MAXLINE 1024

/* ---------------------------------------------------------------------- */

CCStats::CCStats(OINK *oink) : Command(oink)
{
  ninputs = 1;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void CCStats::run()
{
  int me;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  // MRv = Vi : Zi

  MapReduce *mrv = obj->input(1,read,NULL);
  MapReduce *mr = obj->create_mr();

  uint64_t nvert = mr->map(mrv,invert,NULL);
  uint64_t ncc = mr->collate(NULL);
  mr->reduce(count,NULL);
  mr->map(mr,invert,NULL);
  mr->collate(NULL);
  mr->reduce(count,NULL);
  mr->gather(1);
  mr->sort_keys(-1);

  char msg[128];
  sprintf(msg,"CCStats: %lu components, %lu vertices",ncc,nvert);
  if (me == 0) error->message(msg);
  mr->scan(print,NULL);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void CCStats::params(int narg, char **arg)
{
  if (narg != 0) error->all("Illegal cc_stats command");
}

/* ---------------------------------------------------------------------- */

void CCStats::read(int itask, char *file, KeyValue *kv, void *ptr)
{
  char line[MAXLINE];
  VERTEX v,z;

  FILE *fp = fopen(file,"r");
  while (fgets(line,MAXLINE,fp)) {
    sscanf(line,"%lu %lu",&v,&z);
    kv->add((char *) &v,sizeof(VERTEX),(char *) &z,sizeof(VERTEX));
  }
  fclose(fp);
}

/* ---------------------------------------------------------------------- */

void CCStats::print(char *key, int keybytes, char *value,
		    int valuebytes, void *ptr)
{
  uint64_t nsize = *(uint64_t *) key;
  uint64_t ncc = *(uint64_t *) value;
  printf("  %lu CCs with %lu vertices\n",ncc,nsize);
}
