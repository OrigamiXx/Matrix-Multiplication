/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "typedefs.h"
#include "string.h"
#include "stdlib.h"
#include "cc_find.h"
#include "object.h"
#include "style_map.h"
#include "error.h"

#include "blockmacros.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

#define ALLBITS UINT64_MAX
#define INT64MAX INT64_MAX
#define HIBIT UINT64_MAX-INT64_MAX

/* ---------------------------------------------------------------------- */

CCFind::CCFind(OINK *oink) : Command(oink)
{
  ninputs = 1;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void CCFind::run()
{
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  // hardwire a seed for splitting big zones

  int seed = 123456789;
  srand48(seed+me);

  // pshift = amount to left shift a proc ID, to put it 1 bit away from top
  // lmask will mask off hi-bit and proc ID setting in hi-bits of zone

  int pbits = 0;
  while ((1 << pbits) < nprocs) pbits++;
  pshift = 63 - pbits;
  int hbits = pbits + 1;
  lmask = ALLBITS >> hbits;

  // MRe = Eij : NULL

  MapReduce *mre = obj->input(1,read_edge,NULL);
  MapReduce *mrv = obj->create_mr();
  MapReduce *mrz = obj->create_mr();

  // assign each vertex initially to its own zone

  mrv->map(mre,edge_to_vertices,NULL);
  mrv->collate(NULL);
  mrv->reduce(reduce_self_zone,NULL);

  // loop until zones do not change

  int niterate = 0;

  while (1) {
    niterate++;

    mrz->map(mre,map_edge_vert,NULL);
    mrz->add(mrv);
    mrz->collate(NULL);
    mrz->reduce(reduce_edge_zone,NULL);

    mrz->collate(NULL);
    flag = 0;
    mrz->reduce(reduce_zone_winner,this);
    int flagall;
    MPI_Allreduce(&flag,&flagall,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
    if (!flagall) break;

    mrv->map(mrv,map_invert_multi,this);
    mrv->map(mrz,map_zone_multi,this,1);
    mrv->collate(NULL);
    mrv->reduce(reduce_zone_reassign,this);
  }

  // strip any hi-bits from final (Vi,Zi) key/values before output

  mrv->map(mrv,map_strip,NULL);
  obj->output(1,mrv,print,NULL);

  // count # of unique CCs

  mrz->map(mrv,invert,NULL);
  uint64_t ncc = mrz->collate(NULL);

  char msg[128];
  sprintf(msg,"CC_find: %lu components in %d iterations",ncc,niterate);
  if (me == 0) error->message(msg);

  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void CCFind::params(int narg, char **arg)
{
  if (narg != 1) error->all("Illegal cc_find command");
  nthresh = atoi(arg[0]); 
}

/* ---------------------------------------------------------------------- */

void CCFind::print(char *key, int keybytes, 
		    char *value, int valuebytes, void *ptr) 
{
  FILE *fp = (FILE *) ptr;
  VERTEX v = *(VERTEX *) key;
  VERTEX z = *(VERTEX *) &key[sizeof(VERTEX)];
  fprintf(fp,"%lu %lu\n",v,z);
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

void CCFind::reduce_self_zone(char *key, int keybytes,
			      char *multivalue, int nvalues,
			      int *valuebytes, KeyValue *kv, void *ptr)
{
  kv->add(key,keybytes,key,keybytes);
}

/* ---------------------------------------------------------------------- */

void CCFind::map_edge_vert(uint64_t itask, char *key, int keybytes, 
			   char *value, int valuebytes, 
			   KeyValue *kv, void *ptr)
{
  EDGE *edge = (EDGE *) key;
  kv->add((char *) &edge->vi,sizeof(VERTEX),key,sizeof(EDGE));
  kv->add((char *) &edge->vj,sizeof(VERTEX),key,sizeof(EDGE));
}

/* ---------------------------------------------------------------------- */

void CCFind::reduce_edge_zone(char *key, int keybytes,
			      char *multivalue, int nvalues,
			      int *valuebytes, KeyValue *kv, void *ptr)
{
  int i;
  char *value;

  // loop over values to find zone ID

  uint64_t nvalues_total;
  CHECK_FOR_BLOCKS(multivalue,valuebytes,nvalues,nvalues_total)
  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  value = multivalue;
  for (i = 0; i < nvalues; i++) {
    if (valuebytes[i] == sizeof(uint64_t)) break;
    value += valuebytes[i];
  }
  if (i < nvalues) break;

  END_BLOCK_LOOP

  uint64_t zone = *(uint64_t *) value;

  // emit one KV per edge with zone ID as value

  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  value = multivalue;
  for (int i = 0; i < nvalues; i++) {
    if (valuebytes[i] != sizeof(uint64_t))
      kv->add(value,valuebytes[i],(char *) &zone,sizeof(uint64_t));
    value += valuebytes[i];
  }

  END_BLOCK_LOOP
}

/* ---------------------------------------------------------------------- */

void CCFind::reduce_zone_winner(char *key, int keybytes,
				char *multivalue, int nvalues,
				int *valuebytes, KeyValue *kv, void *ptr)
{
  // z0,z1 have hi-bit stripped off

  uint64_t *z = (uint64_t *) multivalue;
  uint64_t z0 = z[0] & INT64MAX;
  uint64_t z1 = z[1] & INT64MAX;

  if (z0 == z1) return;

  // emit zone pair with hi-bits
  // append extra word to value,
  // so zone can be distinguished from vertex values in next stage of CC

  CCFind *data = (CCFind *) ptr;
  data->flag = 1;
  PAD *pad = &(data->pad);

  if (z0 > z1) {
    pad->zone = z[1];
    kv->add((char *) &z[0],sizeof(uint64_t),(char *) pad,sizeof(PAD));
  } else {
    pad->zone = z[0];
    kv->add((char *) &z[1],sizeof(uint64_t),(char *) pad,sizeof(PAD));
  }
}

/* ---------------------------------------------------------------------- */

void CCFind::map_invert_multi(uint64_t itask, char *key, int keybytes, 
			      char *value, int valuebytes, 
			      KeyValue *kv, void *ptr)
{
  uint64_t z = *(uint64_t *) value;

  // if z has hibit set, add random iproc in hibits, retain hibit setting

  if (z >> 63) {
    CCFind *data = (CCFind *) ptr;
    uint64_t iproc = static_cast<uint64_t> (data->nprocs * drand48());
    uint64_t znew = z | (iproc << data->pshift);
    kv->add((char *) &znew,sizeof(uint64_t),key,keybytes);
  } else kv->add(value,valuebytes,key,keybytes);
}

/* ---------------------------------------------------------------------- */

void CCFind::map_zone_multi(uint64_t itask, char *key, int keybytes, 
			    char *value, int valuebytes,
			    KeyValue *kv, void *ptr)
{
  uint64_t z = *(uint64_t *) key;

  // if z has hibit set:
  // remove hibit, add random iproc in hibits, reset hibit

  if (z >> 63) {
    CCFind *data = (CCFind *) ptr;
    uint64_t zstrip = z & INT64MAX;
    kv->add((char *) &zstrip,sizeof(uint64_t),value,valuebytes);
    int nprocs = data->nprocs;
    int pshift = data->pshift;
    uint64_t znew;
    for (uint64_t iproc = 0; iproc < nprocs; iproc++) {
      znew = zstrip | (iproc << pshift);
      znew |= HIBIT;
      kv->add((char *) &znew,sizeof(uint64_t),value,valuebytes);
    }
  } else kv->add(key,keybytes,value,valuebytes);
}

/* ---------------------------------------------------------------------- */

void CCFind::reduce_zone_reassign(char *key, int keybytes,
				  char *multivalue, int nvalues,
				  int *valuebytes, KeyValue *kv, void *ptr)
{
  CCFind *data = (CCFind *) ptr;
  int nthresh = data->nthresh;
  uint64_t lmask = data->lmask;
  
  int i,hnew;
  char *value;
  uint64_t znew;

  // loop over values, compute winning zone ID
  // hibit is set if winning Z has its hibit set

  uint64_t zcount = 0;
  uint64_t zone = *(uint64_t *) key;
  int hkey = zone >> 63;
  zone &= lmask;
  int hwinner = 0;

  uint64_t nvalues_total;
  CHECK_FOR_BLOCKS(multivalue,valuebytes,nvalues,nvalues_total)
  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  value = multivalue;
  for (i = 0; i < nvalues; i++) {
    if (valuebytes[i] != sizeof(uint64_t)) {
      znew = *(uint64_t *) value;
      hnew = znew >> 63;
      znew &= INT64MAX;
      if (znew < zone) {
	zone = znew;
	hwinner = hnew;
      }
      zcount++;
    }
    value += valuebytes[i];
  }

  END_BLOCK_LOOP

  // emit one KV per vertex with zone ID as value
  // add hi-bit to zone if necessary

  if (hkey || hwinner) zone |= HIBIT;
  else if (nvalues_total-zcount > nthresh) zone |= HIBIT;

  BEGIN_BLOCK_LOOP(multivalue,valuebytes,nvalues)

  value = multivalue;
  for (i = 0; i < nvalues; i++) {
    if (valuebytes[i] == sizeof(uint64_t))
      kv->add(value,valuebytes[i],(char *) &zone,sizeof(uint64_t));
    value += valuebytes[i];
  }

  END_BLOCK_LOOP
}

/* ---------------------------------------------------------------------- */

void CCFind::map_strip(uint64_t itask, char *key, int keybytes, 
		       char *value, int valuebytes, KeyValue *kv, void *ptr)
{
  uint64_t zone = *(uint64_t *) value;
  zone &= INT64MAX;
  kv->add(key,keybytes,(char *) &zone,sizeof(uint64_t));
}
