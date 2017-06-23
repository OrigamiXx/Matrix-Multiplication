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
#include "degree_weight.h"
#include "object.h"
#include "style_map.h"
#include "error.h"

#include "blockmacros.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

/* ---------------------------------------------------------------------- */

DegreeWeight::DegreeWeight(OINK *oink) : Command(oink)
{
  ninputs = 2;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void DegreeWeight::run()
{
  int me;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  // MRe = Eij : NULL
  // MRd = Vi : degree

  MapReduce *mre = obj->input(1,read_edge,NULL);
  MapReduce *mrd = obj->input(2,read_vertex_weight,NULL);
  MapReduce *mrewt = obj->create_mr();
  uint64_t nvert = mrd->kv_stats(0);

  mrewt->map(mre,edge_to_vertex_pair,NULL);
  mrewt->add(mrd);
  mrewt->collate(NULL);
  uint64_t nedge = mrewt->reduce(inverse_degree,NULL);
   
  obj->output(1,mrewt,print,NULL);

  char msg[128];
  sprintf(msg,"DegreeWeight: %lu vertices, %lu edges",nvert,nedge);
  if (me == 0) error->message(msg);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void DegreeWeight::params(int narg, char **arg)
{
  if (narg != 0) error->all("Illegal degree_weight command");
}

/* ---------------------------------------------------------------------- */

void DegreeWeight::print(char *key, int keybytes, 
			 char *value, int valuebytes, void *ptr) 
{
  FILE *fp = (FILE *) ptr;
  EDGE *e = (EDGE *) key;
  WEIGHT weight = *(WEIGHT *) value;
  fprintf(fp,"%lu %lu %g\n",e->vi,e->vj,weight);
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

void DegreeWeight::inverse_degree(char *key, int keybytes,
				  char *multivalue, int nvalues,
				  int *valuebytes, KeyValue *kv, void *ptr)
{
  int i;
  char *value;
  EDGE edge;

  uint64_t nvalues_total;
  CHECK_FOR_BLOCKS(multivalue,valuebytes,nvalues,nvalues_total)
  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  // loop over values to find degree count

  value = multivalue;
  for (i = 0; i < nvalues; i++) {
    if (valuebytes[i] == sizeof(int)) break;
    value += valuebytes[i];
  }
  if (i < nvalues) break;

  END_BLOCK_LOOP

  int degree = *(int *) value;
  WEIGHT weight = 1.0/degree;
  edge.vi = *(VERTEX *) key;

  // emit one KV per edge with weight = one/degree as value

  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  value = multivalue;
  for (int i = 0; i < nvalues; i++) {
    if (valuebytes[i] != sizeof(int)) {
      edge.vj = *(VERTEX *) value;
      kv->add((char *) &edge,sizeof(EDGE),(char *) &weight,sizeof(WEIGHT));
    }
    value += valuebytes[i];
  }

  END_BLOCK_LOOP
}

