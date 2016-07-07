/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "typedefs.h"
#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "histo.h"
#include "object.h"
#include "style_map.h"
#include "style_reduce.h"
#include "style_scan.h"
#include "error.h"

#include "mapreduce.h"
#include "keyvalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

/* ---------------------------------------------------------------------- */

Histo::Histo(OINK *oink) : Command(oink)
{
  ninputs = 1;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void Histo::run()
{
  int me,nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  // MR = key : NULL

  MapReduce *mr = obj->input(1);
  uint64_t ntotal = mr->kv_stats(0);

  // unique keys and their count
  // before processing it, make a copy of input MR if it is permanent

  if (obj->permanent(mr)) mr = obj->copy_mr(mr);
  mr->collate(NULL);
  uint64_t nunique = mr->reduce(count,NULL);

  obj->output(1,mr);

  // histogram stats
  // before processing it, make a copy of output MR if it is permanent

  if (obj->permanent(mr)) mr = obj->copy_mr(mr);

  mr->map(mr,invert,NULL);
  mr->collate(NULL);
  mr->reduce(count,NULL);
  mr->gather(1);
  mr->sort_keys(-1);

  char msg[128];
  sprintf(msg,"Histo: %lu total keys, %lu unique",ntotal,nunique);
  if (me == 0) error->message(msg);
  mr->scan(print,NULL);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void Histo::params(int narg, char **arg)
{
  if (narg != 0) error->all("Illegal histo command");
}

/* ---------------------------------------------------------------------- */

void Histo::print(char *key, int keybytes, char *value,
		  int valuebytes, void *ptr)
{
  int ncount = *(int *) key;
  int nkey = *(int *) value;
  printf("  %d keys appear %d times\n",nkey,ncount);
}
