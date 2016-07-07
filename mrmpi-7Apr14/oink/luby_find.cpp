/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "typedefs.h"
#include "string.h"
#include "stdlib.h"
#include "luby_find.h"
#include "object.h"
#include "style_map.h"
#include "error.h"

#include "blockmacros.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

typedef struct {
  uint64_t vi,vj;
} VPAIR;
typedef struct {
  uint64_t v;
  double r;
} VRAND;
typedef struct {
  uint64_t v;
  double r;
  int flag;
} VFLAG;
typedef struct {
  uint64_t vi;
  double ri;
  uint64_t vj;
  double rj;
} ERAND;

/* ---------------------------------------------------------------------- */

LubyFind::LubyFind(OINK *oink) : Command(oink)
{
  ninputs = 1;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void LubyFind::run()
{
  int me;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  // MRe = Eij : NULL

  MapReduce *mre = obj->input(1,read_edge,NULL);
  MapReduce *mrv = obj->create_mr();
  MapReduce *mrw = obj->create_mr();

  // assign a consistent RN to each vertex in each edge
  // convert to KMV

  mrw->map(mre,map_vert_random,&seed);
  mrw->clone();

  // loop until all edges deleted

  int niterate = 0;
  mrv->open();

  while (1) {
    uint64_t n = mrw->reduce(reduce_edge_winner,NULL);
    if (n == 0) break;
    mrw->collate(NULL);
    mrw->reduce(reduce_vert_winner,NULL);
    mrw->collate(NULL);
    mrw->reduce(reduce_vert_loser,NULL);
    mrw->collate(NULL);
    mrw->reduce(reduce_vert_emit,mrv);
    mrw->collate(NULL);
    niterate++;
  }

  uint64_t nset = mrv->close();

  obj->output(1,mrv,print,NULL);

  char msg[128];
  sprintf(msg,"Luby_find: %lu MIS vertices in %d iterations",nset,niterate);
  if (me == 0) error->message(msg);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void LubyFind::params(int narg, char **arg)
{
  if (narg != 1) error->all("Illegal luby_find command");
  seed = atoi(arg[0]); 
}

/* ---------------------------------------------------------------------- */

void LubyFind::print(char *key, int keybytes, 
		     char *value, int valuebytes, void *ptr) 
{
  FILE *fp = (FILE *) ptr;
  VERTEX v = *(VERTEX *) key;
  fprintf(fp,"%lu\n",v);
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

void LubyFind::map_vert_random(uint64_t itask, char *key, int keybytes, 
			       char *value, int valuebytes, 
			       KeyValue *kv, void *ptr)
{
  VPAIR *vpair = (VPAIR *) key;
  ERAND edge;

  int *seed = (int *) ptr;

  edge.vi = vpair->vi;
  srand48(edge.vi + *seed);
  edge.ri = drand48();
  edge.vj = vpair->vj;
  srand48(edge.vj + *seed);
  edge.rj = drand48();
  kv->add((char *) &edge,sizeof(ERAND),NULL,0);
}

/* ---------------------------------------------------------------------- */

void LubyFind::reduce_edge_winner(char *key, int keybytes,
				  char *multivalue, int nvalues,
				  int *valuebytes, KeyValue *kv, void *ptr)
{
  if (nvalues == 2 && (valuebytes[0] || valuebytes[1])) return;

  ERAND *edge = (ERAND *) key;
  int winner;
  if (edge->ri < edge->rj) winner = 0;
  else if (edge->rj < edge->ri) winner = 1;
  else if (edge->vi < edge->vj) winner = 0;
  else winner = 1;

  VRAND v;
  VFLAG vf;
  if (winner == 0) {
    v.v = edge->vi;
    v.r = edge->ri;
    vf.v = edge->vj;
    vf.r = edge->rj;
    vf.flag = 1;
    kv->add((char *) &v,sizeof(VRAND),(char *)&vf,sizeof(VFLAG));
    v.v = edge->vj;
    v.r = edge->rj;
    vf.v = edge->vi;
    vf.r = edge->ri;
    vf.flag = 0;
    kv->add((char *) &v,sizeof(VRAND),(char *) &vf,sizeof(VFLAG));
  } else {
    v.v = edge->vj;
    v.r = edge->rj;
    vf.v = edge->vi;
    vf.r = edge->ri;
    vf.flag = 1;
    kv->add((char *) &v,sizeof(VRAND),(char *) &vf,sizeof(VFLAG));
    v.v = edge->vi;
    v.r = edge->ri;
    vf.v = edge->vj;
    vf.r = edge->rj;
    vf.flag = 0;
    kv->add((char *) &v,sizeof(VRAND),(char *) &vf,sizeof(VFLAG));
  }
}

/* ---------------------------------------------------------------------- */

void LubyFind::reduce_vert_winner(char *key, int keybytes,
				  char *multivalue, int nvalues,
				  int *valuebytes, KeyValue *kv, void *ptr)
{
  int i;
  VFLAG *vf;
  int winflag = 1;

  uint64_t nvalues_total;
  CHECK_FOR_BLOCKS(multivalue,valuebytes,nvalues,nvalues_total)
  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  vf = (VFLAG *) multivalue;
  for (i = 0; i < nvalues; i++) {
    if (vf->flag == 0) {
      winflag = 0;
      break;
    }
    vf++;;
  }
  if (i < nvalues) break;

  END_BLOCK_LOOP

  VRAND *v = (VRAND *) key;
  VRAND v1out,v2out;
  VFLAG vfout;

  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  vf = (VFLAG *) multivalue;
  for (i = 0; i < nvalues; i++) {
    v1out.v = vf->v;
    v1out.r = vf->r;
    if (winflag) {
      vfout.v = v->v;
      vfout.r = v->r;
      vfout.flag = 0;
      kv->add((char *) &v1out,sizeof(VRAND),(char *) &vfout,sizeof(VFLAG));
    } else {
      v2out.v = v->v;
      v2out.r = v->r;
      kv->add((char *) &v1out,sizeof(VRAND),(char *) &v2out,sizeof(VRAND));
    }
    vf++;;
  }

  END_BLOCK_LOOP
}

/* ---------------------------------------------------------------------- */

void LubyFind::reduce_vert_loser(char *key, int keybytes,
				 char *multivalue, int nvalues, 
				 int *valuebytes, KeyValue *kv, void *ptr)
{
  int i;
  int size = 2*sizeof(uint64_t);
  int loseflag = 0;

  uint64_t nvalues_total;
  CHECK_FOR_BLOCKS(multivalue,valuebytes,nvalues,nvalues_total)
  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  for (i = 0; i < nvalues; i++) {
    if (valuebytes[i] > size) {
      loseflag = 1;
      break;
    }
  }
  if (i < nvalues) break;

  END_BLOCK_LOOP

  VRAND *v = (VRAND *) key;
  VFLAG *vf;
  VRAND v1out,v2out;
  VFLAG vfout;

  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  for (i = 0; i < nvalues; i++) {
    vf = (VFLAG *) multivalue;
    v1out.v = vf->v;
    v1out.r = vf->r;
    if (loseflag) {
      vfout.v = v->v;
      vfout.r = v->r;
      vfout.flag = 0;
      kv->add((char *) &v1out,sizeof(VRAND),(char *) &vfout,sizeof(VFLAG));
    } else {
      v2out.v = v->v;
      v2out.r = v->r;
      kv->add((char *) &v1out,sizeof(VRAND),(char *) &v2out,sizeof(VRAND));
    }
    multivalue += valuebytes[i];
  }

  END_BLOCK_LOOP
}

/* ---------------------------------------------------------------------- */

void LubyFind::reduce_vert_emit(char *key, int keybytes,
				char *multivalue, int nvalues, int *valuebytes,
				KeyValue *kv, void *ptr)
{
  int i;
  int size = 2*sizeof(uint64_t);
  int winflag = 1;

  uint64_t nvalues_total;
  CHECK_FOR_BLOCKS(multivalue,valuebytes,nvalues,nvalues_total)
  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  for (i = 0; i < nvalues; i++) {
    if (valuebytes[i] == size) {
      winflag = 0;
      break;
    }
  }
  if (i < nvalues) break;

  END_BLOCK_LOOP

  VRAND *v = (VRAND *) key;
  if (winflag) {
    MapReduce *mrv = (MapReduce *) ptr;
    mrv->kv->add((char *) &v->v,sizeof(uint64_t),NULL,0);
  }

  VFLAG *vf;
  ERAND edge;
  int flag = 0;

  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  for (i = 0; i < nvalues; i++) {
    vf = (VFLAG *) multivalue;
    if (v->v < vf->v) {
      edge.vi = v->v;
      edge.ri = v->r;
      edge.vj = vf->v;
      edge.rj = vf->r;
    } else {
      edge.vi = vf->v;
      edge.ri = vf->r;
      edge.vj = v->v;
      edge.rj = v->r;
    }
    if (valuebytes[i] == size)
      kv->add((char *) &edge,sizeof(ERAND),NULL,0);
    else
      kv->add((char *) &edge,sizeof(ERAND),(char *) &flag,sizeof(int));
    multivalue += valuebytes[i];
  }

  END_BLOCK_LOOP
}
