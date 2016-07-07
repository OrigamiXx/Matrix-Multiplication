#include "typedefs.h"
#include "stdlib.h"
#include "map_rmat_generate.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;

/* ----------------------------------------------------------------------
   rmat_generate
   generate graph edges via recursive R-MAT algorithm
   input: # to generated & R-MAT params extracted from RMAT_struct in ptr
   output: key = Vi Vj, value = NULL
------------------------------------------------------------------------- */

void rmat_generate(int itask, KeyValue *kv, void *ptr)
{
  RMAT_struct *rmat = (RMAT_struct *) ptr;

  uint64_t order = rmat->order;
  uint64_t ngenerate = rmat->ngenerate;
  int nlevels = rmat->nlevels;
  double a = rmat->a;
  double b = rmat->b;
  double c = rmat->c;
  double d = rmat->d;
  double fraction = rmat->fraction;

  uint64_t i,j,delta;
  int ilevel;
  double a1,b1,c1,d1,total,rn;
  EDGE edge;

  for (uint64_t m = 0; m < ngenerate; m++) {
    delta = order >> 1;
    a1 = a; b1 = b; c1 = c; d1 = d;
    i = j = 0;

    for (ilevel = 0; ilevel < nlevels; ilevel++) {
      rn = drand48();
      if (rn < a1) {
      } else if (rn < a1+b1) {
	j += delta;
      } else if (rn < a1+b1+c1) {
	i += delta;
      } else {
	i += delta;
	j += delta;
      }
      
      delta /= 2;
      if (fraction > 0.0) {
	a1 += a1*fraction * (drand48() - 0.5);
	b1 += b1*fraction * (drand48() - 0.5);
	c1 += c1*fraction * (drand48() - 0.5);
	d1 += d1*fraction * (drand48() - 0.5);
	total = a1+b1+c1+d1;
	a1 /= total;
	b1 /= total;
	c1 /= total;
	d1 /= total;
      }
    }

    edge.vi = i;
    edge.vj = j;
    kv->add((char *) &edge,sizeof(EDGE),NULL,0);
  }
}
