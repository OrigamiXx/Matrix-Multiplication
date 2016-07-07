/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "mpi.h"
#include "string.h"
#include "stdlib.h"
#include "wordfreq.h"
#include "object.h"
#include "style_map.h"
#include "style_reduce.h"
#include "style_scan.h"
#include "error.h"

#include "blockmacros.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

struct Count {
  int n,limit,flag;
};

/* ---------------------------------------------------------------------- */

WordFreq::WordFreq(OINK *oink) : Command(oink)
{
  ninputs = 1;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void WordFreq::run()
{
  int me;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  // MR = word : NULL

  int nfiles = 0;
  MapReduce *mr = obj->input(1,read_words,&nfiles);
  uint64_t nwords = mr->kv_stats(0);
  int nfiles_all;
  MPI_Allreduce(&nfiles,&nfiles_all,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

  // unique words and their count
  // before processing it, make a copy of input MR if it is permanent

  if (obj->permanent(mr)) mr = obj->copy_mr(mr);
  mr->collate(NULL);
  uint64_t nunique = mr->reduce(count,NULL);

  obj->output(1,mr,print_string_int,NULL);

  // frequency stats
  // before processing it, make a copy of output MR if it is permanent

  if (ntop) {
    if (obj->permanent(mr)) mr = obj->copy_mr(mr);
    mr->sort_values(-1);

    Count count;
    count.n = 0;
    count.limit = 10;
    count.flag = 0;
    mr->map(mr,output,&count);

    mr->gather(1);
    mr->sort_values(-1);

    count.n = 0;
    count.limit = ntop;
    count.flag = 1;
    mr->map(mr,output,&count);
  }

  char msg[128];
  sprintf(msg,"WordFreq: %d files, %lu words, %lu unique",
  	  nfiles_all,nwords,nunique);
  if (me == 0) error->message(msg);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void WordFreq::params(int narg, char **arg)
{
  if (narg != 1) error->all("Illegal wordfreq command");
  ntop = atoi(arg[0]); 
}

/* ---------------------------------------------------------------------- */

void WordFreq::output(uint64_t itask, char *key, int keybytes, char *value,
		      int valuebytes, KeyValue *kv, void *ptr)
{
  Count *count = (Count *) ptr;
  if (count->n >= count->limit) return;
  count->n++;

  int n = *(int *) value;
  if (count->flag) printf("%d %s\n",n,key);
  else kv->add(key,keybytes,(char *) &n,sizeof(int));
}
