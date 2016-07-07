/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "typedefs.h"
#include "string.h"
#include "stdlib.h"
#include "vertex_extract.h"
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

VertexExtract::VertexExtract(OINK *oink) : Command(oink)
{
  ninputs = 1;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void VertexExtract::run()
{
  int me;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  // MRe = Eij : NULL

  MapReduce *mre = obj->input(1,read_edge_weight,NULL);
  MapReduce *mrv = obj->create_mr();

  // MRv = all vertices in the graph
  
  mrv->map(mre,edge_to_vertices,NULL);
  mrv->collate(NULL);
  mrv->reduce(cull,NULL);

  obj->output(1,mrv,print_vertex,NULL);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void VertexExtract::params(int narg, char **arg)
{
  if (narg != 0) error->all("Illegal VertexExtract command");
}
