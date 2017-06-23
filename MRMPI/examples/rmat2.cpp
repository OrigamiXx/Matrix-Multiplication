/* ----------------------------------------------------------------------
   MR-MPI = MapReduce-MPI library
   http://www.cs.sandia.gov/~sjplimp/mapreduce.html
   Steve Plimpton, sjplimp@sandia.gov, Sandia National Laboratories

   Copyright (2009) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the modified Berkeley Software Distribution (BSD) License.

   See the README file in the top-level MapReduce directory.
------------------------------------------------------------------------- */

// MapReduce random RMAT matrix generation example in C++
// Syntax: rmat N Nz a b c d frac seed {outfile}
//   2^N = # of rows in RMAT matrix
//   Nz = non-zeroes per row
//   a,b,c,d = RMAT params (must sum to 1.0)
//   frac = RMAT randomization param (frac < 1, 0 = no randomization)
//   seed = RNG seed (positive int)
//   outfile = output RMAT matrix to this filename (optional)
//
// Note that this implementation (rmat2.cpp) should have less communication 
// than the original implementation (rmat.cpp) since it uses two MapReduce
// objects to avoid repeatedly communicating edges among processors.

#include "mpi.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "mapreduce.h"
#include "keyvalue.h"
#include <iostream>

using namespace MAPREDUCE_NS;

#define INT_TYPE uint64_t
#define CHECK_FOR_BLOCKS(multivalue, valuebytes, nvalues, totalnvalues)  \
  int macro_nblocks = 1; \
  totalnvalues = nvalues; \
  MapReduce *macro_mr = NULL; \
  if (!(multivalue)) { \
    macro_mr = (MapReduce *) (valuebytes); \
    totalnvalues = macro_mr->multivalue_blocks(macro_nblocks); \
  }


void do_nothing(int, KeyValue *, void *);
void generate(int, KeyValue *, void *);
void cull(char *, int, char *, int, int *, KeyValue *, void *);
void output(char *, int, char *, int, int *, KeyValue *, void *);
void nonzero(char *, int, char *, int, int *, KeyValue *, void *);
void degree(char *, int, char *, int, int *, KeyValue *, void *);
void histo(char *, int, char *, int, int *, KeyValue *, void *);
int ncompare(char *, int, char *, int);
void stats(uint64_t, char *, int, char *, int, KeyValue *, void *);

struct RMAT {            // RMAT params
  int nlevels;           // generate 2^nlevels vertices
  int nnonzero;          // average degree
  INT_TYPE order;        // 2^nlevels
  INT_TYPE ngenerate;    // 2^nlevels*average degree = # edges
  double a,b,c,d,fraction;  // rmat parameters
  char *outfile;
  FILE *fp;
};

typedef INT_TYPE VERTEX;      // vertex ID

typedef struct {         // edge = 2 vertices
  VERTEX vi,vj;
} EDGE;

/* ---------------------------------------------------------------------- */

int main(int narg, char **args)
{
  MPI_Init(&narg,&args);

  int me,nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  // parse command-line args

  if (narg != 9 && narg != 10) {
    if (me == 0) printf("Syntax: rmat N Nz a b c d frac seed {outfile}\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  RMAT rmat;
  rmat.nlevels = atoi(args[1]); 
  rmat.nnonzero = atoi(args[2]); 
  rmat.a = atof(args[3]); 
  rmat.b = atof(args[4]); 
  rmat.c = atof(args[5]); 
  rmat.d = atof(args[6]); 
  rmat.fraction = atof(args[7]); 
  int seed = atoi(args[8]);
  if (narg == 10) {
    int n = strlen(args[9]) + 1;
    rmat.outfile = new char[n];
    strcpy(rmat.outfile,args[9]);
  } else rmat.outfile = NULL;

  if (rmat.a + rmat.b + rmat.c + rmat.d != 1.0) {
    if (me == 0) printf("ERROR: a,b,c,d must sum to 1\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  if (rmat.fraction >= 1.0) {
    if (me == 0) printf("ERROR: fraction must be < 1\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  srand48(seed+me);
  rmat.order = ((INT_TYPE) 1) << rmat.nlevels;

  // loop until desired number of unique nonzero entries
  MapReduce *mrnew = new MapReduce(MPI_COMM_WORLD);
  mrnew->timer = 0;
  MapReduce *mr = NULL;

  MPI_Barrier(MPI_COMM_WORLD);
  double tstart = MPI_Wtime();

  int niterate = 0;
  INT_TYPE ntotal = (((INT_TYPE) 1) << rmat.nlevels) * rmat.nnonzero;
  INT_TYPE nremain = ntotal;
  while (nremain) {
    niterate++;
    rmat.ngenerate = nremain/nprocs;
    if (me < nremain % nprocs) rmat.ngenerate++;
    mrnew->map(nprocs,&generate,&rmat);
    mrnew->aggregate(NULL);
    if (niterate > 1) {
      mr->add(mrnew);
    }
    else {
      mr = mrnew;
      mrnew = new MapReduce(MPI_COMM_WORLD);
      mrnew->timer = 0;
    }
    uint64_t nunique = mr->convert();
    nremain = ntotal - nunique;
    if (me == 0) 
      std::cout << "Iteration " << niterate 
                << " nunique = " << nunique << " of " << ntotal 
                << ";  nremain = " << nremain << std::endl;
    if (nunique == ntotal) break;
    mr->reduce(&cull,&rmat);
  }
  delete mrnew;

  MPI_Barrier(MPI_COMM_WORLD);
  double tstop = MPI_Wtime();

  // output matrix if requested

  if (rmat.outfile) {
    char fname[128];
    sprintf(fname,"%s.%d",rmat.outfile,me);
    rmat.fp = fopen(fname,"w");
    if (rmat.fp == NULL) {
      printf("ERROR: Could not open output file");
      MPI_Abort(MPI_COMM_WORLD,1);
    }
    MapReduce *mr2 = mr->copy();
    mr2->reduce(&output,&rmat);
    fclose(rmat.fp);
    delete mr2;
    if (me == 0) {
      sprintf(fname,"%s.header",rmat.outfile);
      rmat.fp = fopen(fname,"w");
      fprintf(rmat.fp, "%%%%MatrixMarket matrix coordinate real general\n%%\n");
      fprintf(rmat.fp, "%ld %ld %ld\n", rmat.order, rmat.order, ntotal);
      fclose(rmat.fp);
    }
  }

  // stats to screen
  // include stats on number of nonzeroes per row

  if (me == 0) {
    std::cout << rmat.order << " rows in matrix" << std::endl;
    std::cout << ntotal << " nonzeroes in matrix" << std::endl;
  }

  mr->reduce(&nonzero,NULL);
  mr->collate(NULL);
  mr->reduce(&degree,NULL);
  mr->collate(NULL);
  mr->reduce(&histo,NULL);
  mr->gather(1);
  mr->sort_keys(&ncompare);
  INT_TYPE total = 0;
  mr->map(mr,&stats,&total);
  if (me == 0) 
    std::cout << rmat.order-total << " rows with 0 nonzeroes\n" << std::endl;

  if (me == 0)
    std::cout << tstop-tstart << " secs to generate matrix on "
              << nprocs << " procs in " 
              << niterate << " iterations" << std::endl;

  // clean up

  delete mr;
  delete [] rmat.outfile;
  MPI_Finalize();
}

/* ----------------------------------------------------------------------
   We need mr MapReduce object to be in state where it has a KeyValue
   structure, but we don't have anything to put in it yet.  This function 
   will do the trick.
------------------------------------------------------------------------- */
void do_nothing(int itask, KeyValue *kv, void *ptr)
{
}

/* ----------------------------------------------------------------------
   generate RMAT matrix entries
   emit one KV per edge: key = edge, value = NULL
------------------------------------------------------------------------- */

void generate(int itask, KeyValue *kv, void *ptr)
{
  RMAT *rmat = (RMAT *) ptr;

  int nlevels = rmat->nlevels;
  INT_TYPE order = rmat->order;
  INT_TYPE ngenerate = rmat->ngenerate;
  double a = rmat->a;
  double b = rmat->b;
  double c = rmat->c;
  double d = rmat->d;
  double fraction = rmat->fraction;

  INT_TYPE i,j,delta;
  int ilevel;
  double a1,b1,c1,d1,total,rn;
  EDGE edge;

  for (INT_TYPE m = 0; m < ngenerate; m++) {
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

/* ----------------------------------------------------------------------
   eliminate duplicate edges
   input: one KMV per edge, MV has multiple entries if duplicates exist
   output: one KV per edge: key = edge, value = NULL
------------------------------------------------------------------------- */

void cull(char *key, int keybytes, char *multivalue,
	  int nvalues, int *valuebytes, KeyValue *kv, void *ptr) 
{
  kv->add(key,keybytes,NULL,0);
}

/* ----------------------------------------------------------------------
   write edges to a file unique to this processor
------------------------------------------------------------------------- */

void output(char *key, int keybytes, char *multivalue,
	    int nvalues, int *valuebytes, KeyValue *kv, void *ptr) 
{
  RMAT *rmat = (RMAT *) ptr;
  EDGE *edge = (EDGE *) key;
  fprintf(rmat->fp,"%ld %ld 1\n",edge->vi+1,edge->vj+1);
}

/* ----------------------------------------------------------------------
   enumerate nonzeroes in each row
   input: one KMV per edge
   output: one KV per edge: key = row I, value = NULL
------------------------------------------------------------------------- */

void nonzero(char *key, int keybytes, char *multivalue,
	     int nvalues, int *valuebytes, KeyValue *kv, void *ptr) 
{
  EDGE *edge = (EDGE *) key;
  kv->add((char *) &edge->vi,sizeof(VERTEX),NULL,0);
}

/* ----------------------------------------------------------------------
   count nonzeroes in each row
   input: one KMV per row, MV has entry for each nonzero
   output: one KV: key = # of nonzeroes, value = NULL
------------------------------------------------------------------------- */

void degree(char *key, int keybytes, char *multivalue,
	 int nvalues, int *valuebytes, KeyValue *kv, void *ptr) 
{
  uint64_t total_nvalues;
  CHECK_FOR_BLOCKS(multivalue, valuebytes, nvalues, total_nvalues);
  kv->add((char *) &total_nvalues,sizeof(uint64_t),NULL,0);
}

/* ----------------------------------------------------------------------
   count rows with same # of nonzeroes
   input: one KMV per nonzero count, MV has entry for each row
   output: one KV: key = # of nonzeroes, value = # of rows
------------------------------------------------------------------------- */

void histo(char *key, int keybytes, char *multivalue,
	   int nvalues, int *valuebytes, KeyValue *kv, void *ptr) 
{
  uint64_t total_nvalues;
  CHECK_FOR_BLOCKS(multivalue, valuebytes, nvalues, total_nvalues);
  kv->add(key,keybytes,(char *) &total_nvalues,sizeof(uint64_t));
}

/* ----------------------------------------------------------------------
   compare two counts
   order values by count, largest first
------------------------------------------------------------------------- */

int ncompare(char *p1, int len1, char *p2, int len2)
{
  uint64_t i1 = *(uint64_t *) p1;
  uint64_t i2 = *(uint64_t *) p2;
  if (i1 > i2) return -1;
  else if (i1 < i2) return 1;
  else return 0;
}

/* ----------------------------------------------------------------------
   print # of rows with a specific # of nonzeroes
------------------------------------------------------------------------- */

void stats(uint64_t itask, char *key, int keybytes, char *value,
	   int valuebytes, KeyValue *kv, void *ptr)
{
  uint64_t *total = (uint64_t *) ptr;
  uint64_t nnz = *(uint64_t *) key;
  uint64_t ncount = *(uint64_t *) value;
  *total += ncount;
  std::cout << ncount << " rows with " << nnz << " nonzeros" << std::endl;
}
