/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "typedefs.h"
#include "mpi.h"
#include "string.h"
#include "stdlib.h"
#include "edge_upper.h"
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

/* ---------------------------------------------------------------------- */

EdgeUpper::EdgeUpper(OINK *oink) : Command(oink)
{
  ninputs = 1;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void EdgeUpper::run()
{
  int me;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  // MRe = Eij : NULL

  MapReduce *mre = obj->input(1,read_edge,NULL);
  MapReduce *mr = obj->create_mr();
  uint64_t nedge = mre->kv_stats(0);

  mr->map(mre,edge_upper,NULL);
  mr->collate(NULL);
  uint64_t unique = mr->reduce(cull,NULL);
   
  obj->output(1,mr,print_edge,NULL);

  char msg[128];
  sprintf(msg,"EdgeUpper: %lu original edges, %lu final edges",
	  nedge,unique);
  if (me == 0) error->message(msg);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void EdgeUpper::params(int narg, char **arg)
{
  if (narg != 0) error->all("Illegal edge_upper command");
}
