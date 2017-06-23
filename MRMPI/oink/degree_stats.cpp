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
#include "degree_stats.h"
#include "object.h"
#include "style_map.h"
#include "style_reduce.h"
#include "error.h"

#include "mapreduce.h"
#include "keyvalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

/* ---------------------------------------------------------------------- */

DegreeStats::DegreeStats(OINK *oink) : Command(oink)
{
  ninputs = 1;
  noutputs = 0;
}

/* ---------------------------------------------------------------------- */

void DegreeStats::run()
{
  int me,nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  // MRe = Eij : NULL

  MapReduce *mre = obj->input(1,read_edge,NULL);
  MapReduce *mr = obj->create_mr();
  uint64_t nedge = mre->kv_stats(0);

  if (duplicate == 1) mr->map(mre,edge_to_vertex,NULL);
  else mr->map(mre,edge_to_vertices,NULL);
  mr->collate(NULL);
  uint64_t nvert = mr->reduce(count,NULL);

  mr->map(mr,invert,NULL);
  mr->collate(NULL);
  mr->reduce(count,NULL);
  mr->gather(1);
  mr->sort_keys(-1);

  char msg[128];
  sprintf(msg,"DegreeStats: %lu vertices, %lu edges",nvert,nedge);
  if (me == 0) error->message(msg);
  mr->scan(print,NULL);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void DegreeStats::params(int narg, char **arg)
{
  if (narg != 1) error->all("Illegal degree_stats command");

  duplicate = atoi(arg[0]);
}

/* ---------------------------------------------------------------------- */

void DegreeStats::print(char *key, int keybytes, char *value,
			int valuebytes, void *ptr)
{
  int nedge = *(int *) key;
  int nvert = *(int *) value;
  printf("  %d vertices with %d edges\n",nvert,nedge);
}
