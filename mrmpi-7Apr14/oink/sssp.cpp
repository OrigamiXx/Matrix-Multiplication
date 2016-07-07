/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "typedefs.h"
#include "string.h"
#include "stdlib.h"
#include "sssp.h"
#include "object.h"
#include "style_map.h"
#include "style_reduce.h"
#include "error.h"

#include <iostream>
#include "blockmacros.h"
#include "mapreduce.h"
#include "keyvalue.h"
#include "keymultivalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;
using namespace std;

#define MAX_NUM_EXPERIMENTS 50

#ifdef NOISY
#define HELLO {std::cout << "KDD " << __func__ << std::endl;}
#else
#define HELLO 
#endif

uint64_t SSSP::NVtxLabeled = 0;

/////////////////////////////////////////////////////////////////////////////
SSSP::SSSP(OINK *oink) : Command(oink)
{ HELLO

  MPI_Comm_rank(MPI_COMM_WORLD, &me);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  ninputs = 1;
  noutputs = 1;
}

/////////////////////////////////////////////////////////////////////////////
void SSSP::run()
{ HELLO
  int me;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  srand48(seed);

  // MRe = Eij : weight

  MapReduce *mredge = obj->input(1,read_edge_weight,NULL);

  // Create list of vertices from the edges.  
  // The resulting graph has no singleton vertices.
  // The reduce operation pre-aggregates the vertices to processors as needed.
  MapReduce *mrvert = obj->create_mr(); 
  mrvert->map(mredge,edge_to_vertices,NULL);
  mrvert->collate(NULL);
  mrvert->reduce(cull,NULL);

  // Generate a list of valid sources.  Assume no singletons in mrvert.
  MapReduce *mrlist = mrvert->copy();  // vertices are already aggregated.
  mrlist->compress(get_good_sources, this);
  delete mrlist;

  // Aggregate mredge by source-vertex of each edge.  No need to convert yet.
  // mredge and mrvert, then, are essentially local mapreduce objects.
  mredge->map(mredge, reorganize_edges, NULL);
  mredge->aggregate(NULL);  

  // Select a source vertex.  
  //       Processor 0 selects random number S in range [1:N] for N vertices.
  //       Processor 0 emits into Paths key-value pair [S, {-1, 0}], 
  //       signifying that vertex S has distance zero from itself, with no
  //       predecessor.

  double tcompute = 0;
  MPI_Barrier(MPI_COMM_WORLD);
  double tstart = MPI_Wtime();

  for (int cnt = 0; cnt < ncnt; cnt++) {
    VERTEX source;
    if (!get_next_source(&source, cnt))
      break;

    // Initialize vertex distances.
    mrvert->map(mrvert, initialize_vertex_distances, (void *) NULL, 0);

    MapReduce *mrpath = obj->create_mr();
    if (me == 0)
      std::cout << cnt << ": BEGINNING SOURCE " << source << std::endl;

    mrpath->map(1, add_source, &source);

    //  Perform a BFS from S, editing distances as visit vertices.
    int done = 0;
    int iter = 0;
    while (!done) {
      done = 1;

      // First, determine which, if any, vertex distances have changed.
      // Add updated distances existing distances.
      mrpath->aggregate(NULL);

      mrvert->kv->append();
      mrpath->map(mrpath, move_to_new_mr, mrvert);
      mrvert->kv->complete();

      // Pick best distances.  For vertices with changed distances,
      // emit new distances into mrpath.
      uint64_t tmp_nv = 0, tmp_ne = 0;
      NVtxLabeled = 0;
      mrpath->kv->append();
      tmp_nv = mrvert->compress(pick_shortest_distances, mrpath);
      mrpath->kv->complete();

      uint64_t nchanged;
      MPI_Allreduce(&(mrpath->kv->nkv), &nchanged, 1, MPI_UNSIGNED_LONG,
                    MPI_SUM, MPI_COMM_WORLD);
      if (nchanged) {
        // Some vtxs' distances changed; 
        // need to emit new distances for adjacent vtxs.
        done = 0;
        mredge->kv->append();
        mrpath->map(mrpath, move_to_new_mr, mredge);
        mredge->kv->complete();

        mrpath->kv->append();
        tmp_ne = mredge->compress(update_adjacent_distances, mrpath);
        mrpath->kv->complete();
      }
      else done = 1;

      int alldone;
      MPI_Allreduce(&done, &alldone, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
      done = alldone;

      if (me == 0)
        std::cout << "   Iteration " << iter
                  << " MRPath size " << mrpath->kv->nkv
                  << " MRVert size " << mrvert->kv->nkv 
                  << " MREdge size " << mredge->kv->nkv
                  << std::endl;
      iter++;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double tstop = MPI_Wtime();
    tcompute += (tstop - tstart);

    uint64_t GNVtxLabeled;
    MPI_Allreduce(&NVtxLabeled, &GNVtxLabeled, 1, MPI_UNSIGNED_LONG, 
                  MPI_SUM, MPI_COMM_WORLD);
    if (me == 0) {
      std::cout << cnt << ":  Source = " << source
                << "; Iterations = " << iter 
                << "; Num Vtx Labeled = " << GNVtxLabeled << std::endl;
      std::cout << cnt << ":  Source = " << source
                << "; Iterations = " << iter 
                << "; Compute Time = " << (tstop-tstart) << std::endl;
    }

    // Now mrpath contains one key-value per vertex Vi:
    // Key = Vi, Value = DISTANCE = {predecessor, weight}

    obj->output(1,mrpath,print,NULL);
  }

  if (me == 0) 
    std::cout << "Total time in SSSP: " << tcompute << std::endl;

  obj->cleanup();
}


/////////////////////////////////////////////////////////////////////////////
// reorganize_edges:  Just reorganize the way edge data is presented.
// Map:    Input:   edge data with key=(Vi,Vj), value=wt
//         Output:  edge data with key=Vi, edge=(Vj,wt)
void SSSP::reorganize_edges(uint64_t itask, char *key, int keybytes, 
                            char *value, int valuebytes, 
                            KeyValue *kv, void *ptr)
{ HELLO
  EDGE *e = (EDGE *) key;
  VERTEX v = e->vi;
  EDGEVALUE ev;
  ev.v = e->vj;
  ev.wt = *(WEIGHT *)value;

  kv->add((char *) &v, sizeof(VERTEX), (char *) &ev, sizeof(EDGEVALUE));

}

/////////////////////////////////////////////////////////////////////////////
// add_source:  Add the source vertex to the MapReduce object as initial vtx.
// Map:    Input:   randomly selected vertex in [1:N] for source.
//         Output:  One key-value pair for the source.
void SSSP::add_source(int nmap, KeyValue *kv, void *ptr)
{ HELLO
  VERTEX *v = (VERTEX *) ptr;
  DISTANCE d;
  d.e.wt = 0;  // Distance from source to itself is zero.
  d.current = false;
  kv->add((char *) v, sizeof(VERTEX),
          (char *) &d, sizeof(DISTANCE));
}

/////////////////////////////////////////////////////////////////////////////
// move_to_new_mr:  Move KV from existing MR to new MR provided in ptr.
// Map:    Input:   KVs in exisitng MR object, new MR object in ptr.
//         Output:  No KVs in existing MR object; they are all added to new MR.
void SSSP::move_to_new_mr(uint64_t itask, char *key, int keybytes, 
                          char *value, int valuebytes, 
                          KeyValue *kv, void *ptr)
{ HELLO
  MapReduce *mr = (MapReduce *) ptr;
  mr->kv->add(key, keybytes, value, valuebytes);
}

/////////////////////////////////////////////////////////////////////////////
// initialize_vertex_distances:  Add initial distance to all vertices.
// Map:    Input:   KV  key = Vtx ID; value = NULL
//         Output:  KV  key = Vtx ID; value = initial distance
void SSSP::initialize_vertex_distances(uint64_t itask, char *key, int keybytes, 
                                       char *value, int valuebytes, 
                                       KeyValue *kv, void *ptr)
{ HELLO
  DISTANCE d;
  kv->add(key, keybytes, (char *) &d, sizeof(DISTANCE));
}


/////////////////////////////////////////////////////////////////////////////
// pick_shortest_distances:  For each vertex, pick the best distance.
// Emit the winning distance.  Also, emit any changed vertex distances
// to mrpath (in the ptr argument).
void SSSP::pick_shortest_distances(char *key, int keybytes, char *multivalue,
                                   int nvalues, int *valuebytes, 
                                   KeyValue *kv, void *ptr)
{ HELLO
  MapReduce *mrpath = (MapReduce *) ptr;

  uint64_t total_nvalues;
  CHECK_FOR_BLOCKS(multivalue, valuebytes, nvalues, total_nvalues)

  DISTANCE shortest; // Shortest path so far to Vi.
  DISTANCE previous; // Previous best answer.
  bool modified = false;

  if (total_nvalues > 1) {
    // Need to find the shortest distance to Vi.

    BEGIN_BLOCK_LOOP(multivalue, valuebytes, nvalues)

    uint64_t offset = 0;
    for (int j = 0; j < nvalues; j++) {
      DISTANCE *d = (DISTANCE*) (multivalue+offset);
      if (d->e.wt < shortest.e.wt) {
        shortest = *d;   // shortest path so far.
      }
      if (d->current) previous = *d;
      offset += valuebytes[j];
    }

    END_BLOCK_LOOP
  }
  else {
    DISTANCE *d = (DISTANCE*) multivalue;
    shortest = *d;
    previous = *d;
  } 

  // Did we change the vertex's distance?
  if (previous != shortest) modified = true;

  // Emit vertex with updated distance back into mrvert.
  shortest.current = true;
  kv->add(key, keybytes, (char *) &shortest, sizeof(DISTANCE));
  if (shortest.e.wt < FLT_MAX) NVtxLabeled++;

  // If changes were made, emit the new distance into mrpath.
  if (modified) {
    mrpath->kv->add(key, keybytes,
                   (char *) &shortest, sizeof(DISTANCE));
  }
}

/////////////////////////////////////////////////////////////////////////////
// update_adjacent_distances:  For each vertex whose distance has changed, 
// emit into mrpath a possible updated distance to each of its adjacencies.
// Also emit the adjacency list back into mredge.
void SSSP::update_adjacent_distances(char *key, int keybytes, char *multivalue,
                                     int nvalues, int *valuebytes, 
                                     KeyValue *kv, void *ptr)
{ HELLO
  MapReduce *mrpath = (MapReduce *) ptr;
  VERTEX *vi = (VERTEX *) key;
  bool found = false;
  DISTANCE shortest;

  uint64_t total_nvalues;
  CHECK_FOR_BLOCKS(multivalue, valuebytes, nvalues, total_nvalues)

  // Find the updated distance, if any.
  // Also, re-emit the edges into mredge.
  BEGIN_BLOCK_LOOP(multivalue, valuebytes, nvalues)

  uint64_t offset = 0;
  for (int j = 0; j < nvalues; j++) {
    // Multivalues are either edges or distances.  Distances use more bytes.
    if (valuebytes[j] == sizeof(DISTANCE)) {
      // This is a distance value.
      DISTANCE *d = (DISTANCE*) (multivalue+offset);
      found = true;
      if (d->e.wt < shortest.e.wt)  shortest = *d;   // shortest path so far.
    }
    else {
      // This is an edge value.  Re-emit it into mredge.
      kv->add(key, keybytes, multivalue+offset, valuebytes[j]);
    }
    offset += valuebytes[j];
  }

  END_BLOCK_LOOP

  // If an updated distance was found, need to update distances for 
  // outward adjacencies.  Add these updates to mrpath.
  if (found) {
    BEGIN_BLOCK_LOOP(multivalue, valuebytes, nvalues)

    uint64_t offset = 0;
    for (int j = 0; j < nvalues; j++) {
      // Multivalues are either edges or distances.  Distances use more bytes.
      if (valuebytes[j] == sizeof(EDGEVALUE)) {
        // This is an edge value.  Emit the updated distance.
        EDGEVALUE *e = (EDGEVALUE *) (multivalue+offset);

        // with all wt > 0, don't follow (1) loops back to predecessor or
        // (2) self-loops.
        if ((shortest.e.v != e->v) && (e->v != *vi)) {
          DISTANCE dist;
          dist.e.v = *vi;    // Predecessor of Vj along the path.
          dist.e.wt = shortest.e.wt + e->wt;
          dist.current = false;
          mrpath->kv->add((char *) &(e->v), sizeof(VERTEX),
                          (char *) &dist, sizeof(DISTANCE));
        }
      }
      offset += valuebytes[j];
    }
    END_BLOCK_LOOP
  }
}

/////////////////////////////////////////////////////////////////////////////
void SSSP::get_good_sources(char *key, int keybytes, char *multivalue,
                            int nvalues, int *valuebytes, 
                            KeyValue *kv, void *ptr)
{ HELLO
class SSSP *ths = (class SSSP *) ptr;

  // Check whether already have enough sources.
  if (ths->sourcelist.size() >= ths->ncnt) return;

  // If vertex made it to this routine, it has non-zero outdegree.  Use it
  // as a source.
  ths->sourcelist.push_back(*((VERTEX *) key));
}


/////////////////////////////////////////////////////////////////////////////
bool SSSP::get_next_source(
  VERTEX *source,
  int cnt
)
{ HELLO
  *source = 0;
  if (me == 0) {
    if (cnt < sourcelist.size())
      *source = sourcelist[cnt];
  }
  MPI_Bcast(source, sizeof(VERTEX), MPI_BYTE, 0, MPI_COMM_WORLD);
  return(*source != 0);
}

/* ---------------------------------------------------------------------- */

void SSSP::params(int narg, char **arg)
{ HELLO
  if (narg != 2) error->all("Illegal sssp command");

  ncnt = atoi(arg[0]);  std::cout << "PARAM ncnt=" << ncnt << std::endl;
  seed = atoi(arg[1]);  std::cout << "PARAM seed=" << seed << std::endl;
}

/* ---------------------------------------------------------------------- */

void SSSP::print(char *key, int keybytes, 
		 char *value, int valuebytes, void *ptr) 
{ HELLO
  FILE *fp = (FILE *) ptr;
  VERTEX *v = (VERTEX *) key;
  DISTANCE *d = (DISTANCE *) value;
  fprintf(fp,"%lu %g %lu\n", v, d->e.wt, d->e.v);
}

/* ---------------------------------------------------------------------- */
