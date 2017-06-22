/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "typedefs.h"
#include "string.h"
#include "stdlib.h"
#include "pagerank.h"
#include "object.h"
#include "style_map.h"
#include "style_reduce.h"
#include "error.h"

#include "blockmacros.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

/* ---------------------------------------------------------------------- */

PageRank::PageRank(OINK *oink) : Command(oink)
{
  ninputs = 1;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void PageRank::run()
{
  int me;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  // MRe = Eij : weight

  MapReduce *mre = obj->input(1,read_edge_weight,NULL);
  MapReduce *mrv = obj->create_mr();
  MapReduce *mrr = obj->create_mr();

  // MRv = list of vertices in the directed graph
  
  mrv->map(mre,edge_to_vertices,NULL);
  mrv->collate(NULL);
  mrv->reduce(cull,NULL);

  // iterate over PageRank calculations

  for (int iterate = 0; iterate < maxiter; iterate++) {

  }

  // MRr = Vi : rank

  //obj->output(1,mrr,print,NULL);
  obj->output(1,mre,print,NULL);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void PageRank::params(int narg, char **arg)
{
  if (narg != 3) error->all("Illegal pagerank command");

  tolerance = atof(arg[0]);
  maxiter = atoi(arg[1]);
  alpha = atof(arg[2]);
}

/* ---------------------------------------------------------------------- */

void PageRank::print(char *key, int keybytes, 
		 char *value, int valuebytes, void *ptr) 
{
  FILE *fp = (FILE *) ptr;
  VERTEX v = *(VERTEX *) key;
  double rank = *(double *) value;
  fprintf(fp,"%lu %g\n",v,rank);
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

// here are sample map/reduce funcs

/*

void PageRank::map_edge_vert(uint64_t itask, char *key, int keybytes, 
			    char *value, int valuebytes, 
			    KeyValue *kv, void *ptr)
{
}

void PageRank::reduce_second_degree(char *key, int keybytes,
				char *multivalue, int nvalues, 
				int *valuebytes, KeyValue *kv, void *ptr)
{
}

*/
