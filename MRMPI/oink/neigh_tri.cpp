/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "typedefs.h"
#include "string.h"
#include "stdlib.h"
#include "neigh_tri.h"
#include "object.h"
#include "error.h"

#include "blockmacros.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

typedef struct {
  VERTEX vi,vj,vk;
} TRI;

#define MAXLINE 1024*1024

/* ---------------------------------------------------------------------- */

NeighTri::NeighTri(OINK *oink) : Command(oink)
{
  ninputs = 2;
  noutputs = 1;
}

/* ---------------------------------------------------------------------- */

void NeighTri::run()
{
  int me;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  // MRn = Vi : Vj Vk ...
  // MRt = Vi Vj Vk : NULL

  char line[MAXLINE];
  MapReduce *mrn = obj->input(1,nread,line);
  MapReduce *mrt = obj->input(2,tread,NULL);
  MapReduce *mrnplus = obj->copy_mr(mrn);

  mrnplus->map(mrt,map1,NULL,1);
  mrnplus->collate(NULL);
  mrnplus->scan(print,this);

  obj->output(1,mrnplus);

  delete [] dirname;
  obj->cleanup();
}

/* ---------------------------------------------------------------------- */

void NeighTri::params(int narg, char **arg)
{
  if (narg != 1) error->all("Illegal neigh_tri command");

  int n = strlen(arg[0]) + 1;
  dirname = new char[n];
  strcpy(dirname,arg[0]);
}

/* ---------------------------------------------------------------------- */

void NeighTri::nread(int itask, char *file, KeyValue *kv, void *ptr)
{
  VERTEX vi,vj;
  char *line = (char *) ptr;

  char *pvj;
  FILE *fp = fopen(file,"r");
  while (fgets(line,MAXLINE,fp)) {
    vi = atoll(strtok(line," \t\n"));
    while (pvj = strtok(NULL," \t\n")) {
      vj = atoll(pvj);
      kv->add((char *) &vi,sizeof(VERTEX),(char *) &vj,sizeof(VERTEX));
    }
  }
  fclose(fp);
}

/* ---------------------------------------------------------------------- */

void NeighTri::tread(int itask, char *file, KeyValue *kv, void *ptr)
{
  char line[MAXLINE];
  TRI tri;

  FILE *fp = fopen(file,"r");
  while (fgets(line,MAXLINE,fp)) {
    sscanf(line,"%lu %lu %lu",&tri.vi,&tri.vj,&tri.vk);
    kv->add((char *) &tri,sizeof(TRI),NULL,0);
  }
  fclose(fp);
}

/* ---------------------------------------------------------------------- */

void NeighTri::print(char *key, int keybytes, char *multivalue, int nvalues,
		     int *valuebytes, void *ptr) 
{
  VERTEX vi,vj,vk;

  vi = *(VERTEX *) key;

  NeighTri *nt = (NeighTri *) ptr;
  char fname[128];
  sprintf(fname,"%s/%lu",nt->dirname,vi);
  FILE *fp = fopen(fname,"w");
  if (fp == NULL) {
    Error *error = nt->error;
    error->one("Could not open file in print");
  }

  int offset = 0;
  for (int i = 0; i < nvalues; i++) {
    if (valuebytes[i] == sizeof(VERTEX)) {
      vj = *(VERTEX *) &multivalue[offset];
      fprintf(fp,"%lu %lu\n",vi,vj);
    } else {
      vj = *(VERTEX *) &multivalue[offset];
      vk = *(VERTEX *) &multivalue[offset+sizeof(VERTEX)];
      fprintf(fp,"%lu %lu\n",vj,vk);
    }
    offset += valuebytes[i];
  }
  fclose(fp);
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

void NeighTri::map1(uint64_t itask, char *key, int keybytes, 
		    char *value, int valuebytes, KeyValue *kv, void *ptr) 
{
  EDGE edge;
  TRI *tri = (TRI *) key;

  edge.vi = tri->vj;
  edge.vj = tri->vk;
  kv->add((char *) &tri->vi,sizeof(VERTEX),(char *) &edge,sizeof(EDGE));

  edge.vi = tri->vi;
  edge.vj = tri->vk;
  kv->add((char *) &tri->vj,sizeof(VERTEX),(char *) &edge,sizeof(EDGE));

  edge.vi = tri->vi;
  edge.vj = tri->vj;
  kv->add((char *) &tri->vk,sizeof(VERTEX),(char *) &edge,sizeof(EDGE));
}
