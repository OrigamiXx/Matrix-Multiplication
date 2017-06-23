/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "typedefs.h"
#include "mpi.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "rmat2.h"
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

RMAT2::RMAT2(OINK *oink) : Command(oink)
{
  ninputs = 0;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void RMAT2::run()
{
  int me,nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  // mr = matrix edges

  MapReduce *mr = obj->create_mr();
  MapReduce *mrnew = obj->create_mr();

  // loop until desired number of unique nonzero entries

  int niterate = 0;
  uint64_t ntotal = rmat.order * rmat.nnonzero;
  uint64_t nremain = ntotal;

  while (nremain) {
    niterate++;
    rmat.ngenerate = nremain/nprocs;
    if (me < nremain % nprocs) rmat.ngenerate++;
    mrnew->map(nprocs,rmat_generate,&rmat);
    mrnew->aggregate(NULL);
    mr->add(mrnew);
    uint64_t nunique = mr->convert();
    mr->reduce(cull,&rmat);
    nremain = ntotal - nunique;
  }

  obj->output(1,mr,print_edge,NULL);

  char msg[128];
  sprintf(msg,"RMAT2: %lu rows, %lu non-zeroes, %d iterations",
	  rmat.order,ntotal,niterate);
  if (me == 0) error->message(msg);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void RMAT2::params(int narg, char **arg)
{
  if (narg != 8) error->all("Illegal rmat command");

  rmat.nlevels = atoi(arg[0]); 
  rmat.nnonzero = atoi(arg[1]); 
  rmat.a = atof(arg[2]); 
  rmat.b = atof(arg[3]); 
  rmat.c = atof(arg[4]); 
  rmat.d = atof(arg[5]); 
  rmat.fraction = atof(arg[6]); 
  int seed = atoi(arg[7]);

  if (rmat.a + rmat.b + rmat.c + rmat.d != 1.0)
    error->all("RMAT a,b,c,d must sum to 1");
  if (rmat.fraction >= 1.0) error->all("RMAT fraction must be < 1");

  int me;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  srand48(seed+me);
  rmat.order = 1 << rmat.nlevels;
}
