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

#ifndef MAP_REDUCE_H
#define MAP_REDUCE_H

#include "mpi.h"
#include "stdint.h"

namespace MAPREDUCE_NS {

class MapReduce {
  friend class KeyValue;
  friend class KeyMultiValue;
  friend class Spool;

 public:
  int mapstyle;       // 0 = chunks, 1 = strided, 2 = master/slave
  int all2all;        // 0 = irregular comm, 1 = use MPI_Alltoallv()
  int verbosity;      // 0 = none, 1 = totals, 2 = proc histograms
  int timer;          // 0 = none, 1 = summary, 2 = proc histograms
  int memsize;        // # of Mbytes per page
  int minpage;        // # of pages that will be pre-allocated per proc >= 0
  int maxpage;        // max # of pages that can be allocated per proc, 0 = inf
  int freepage;       // 1 to free unused pages after every operation, 0 if keep
  int outofcore;      // 1 to force data out-of-core, 0 = only if exceeds 1 pg
  int zeropage;       // 1 to init allocated pages to 0, 0 if don't bother
  int keyalign;       // align keys to this byte count
  int valuealign;     // align values to this byte count
  char *fpath;        // prefix path added to intermediate out-of-core files
  int mapfilecount;   // number of files processed by map file variants

  class KeyValue *kv;              // single KV stored by MR
  class KeyMultiValue *kmv;        // single KMV stored by MR

  // static variables across all MR objects

  static MapReduce *mrptr;         // holds a ptr to MR currently being used
  static int instances_now;        // total # of MRs currently instantiated
                                   // grows as created, shrinks as destroyed
  static int instances_ever;       // total # of MRs ever instantiated
                                   // grows as created, never shrinks
  static int mpi_finalize_flag;    // 1 if MR library should finalize MPI
  static uint64_t msize,msizemax;  // current and hi-water memory allocation
  static uint64_t rsize,wsize;     // total read/write bytes for all I/O
  static uint64_t cssize,crsize;   // total send/recv bytes for all comm
  static double commtime;          // total time for all comm

  // library API

  MapReduce(MPI_Comm);
  MapReduce();
  MapReduce(double);
  ~MapReduce();

  MapReduce *copy();

  uint64_t add(MapReduce *);
  uint64_t aggregate(int (*)(char *, int));
  uint64_t broadcast(int);
  uint64_t clone();
  uint64_t close();
  uint64_t collapse(char *, int);
  uint64_t collate(int (*)(char *, int));
  uint64_t compress(void (*)(char *, int, char *,
			     int, int *, class KeyValue *, void *),
		    void *);
  uint64_t convert();
  uint64_t gather(int);

  uint64_t map(int, void (*)(int, class KeyValue *, void *),
	       void *, int addflag = 0);
  uint64_t map(int, char **, int, int, int,
	       void (*)(int, char *, class KeyValue *, void *),
	       void *, int addflag = 0);
  uint64_t map(int, int, char **, int, int, char, int, 
	       void (*)(int, char *, int, class KeyValue *, void *),
	       void *, int addflag = 0);
  uint64_t map(int, int, char **, int, int, char *, int, 
	       void (*)(int, char *, int, class KeyValue *, void *),
	       void *, int addflag = 0);
  uint64_t map(MapReduce *, void (*)(uint64_t, char *, int, char *, int, 
				     class KeyValue *, void *),
	       void *, int addflag = 0);

  void open(int addflag = 0);
  void print(int, int, int, int);
  void print(char *, int, int, int, int, int);
  uint64_t reduce(void (*)(char *, int, char *,
			   int, int *, class KeyValue *, void *), void *);
  uint64_t scan(void (*)(char *, int, char *, int, void *), void *);
  uint64_t scan(void (*)(char *, int, char *, int, int *, void *), void *);
  uint64_t scrunch(int, char *, int);

  uint64_t multivalue_blocks(int &);
  void multivalue_block_select(int);
  int multivalue_block(int, char **, int **);

  uint64_t sort_keys(int);
  uint64_t sort_keys(int (*)(char *, int, char *, int));
  uint64_t sort_values(int);
  uint64_t sort_values(int (*)(char *, int, char *, int));
  uint64_t sort_multivalues(int);
  uint64_t sort_multivalues(int (*)(char *, int, char *, int));

  uint64_t kv_stats(int);
  uint64_t kmv_stats(int);
  void cummulative_stats(int, int);

  void set_fpath(const char *);

  // query functions

  MPI_Comm communicator() {return comm;};
  int num_procs() {return nprocs;};
  int my_proc() {return me;};

  // functions accessed thru non-class wrapper functions

  void map_file_wrapper(int, class KeyValue *);
  int compare_wrapper(int, int);

 private:
  MPI_Comm comm;
  int me,nprocs;
  int instance_me;         // which instances_ever I am
  int allocated;
  double time_start,time_stop;
  class Memory *memory;
  class Error *error;

  uint64_t rsize_one,wsize_one;     // file read/write bytes for one operation
  uint64_t crsize_one,cssize_one;   // send/recv comm bytes for one operation

  int collateflag;          // flag for when convert() is called from collate()

  // memory pages and bookkeeping

  uint64_t pagesize;        // pagesize for KVs and KMVs
  char **memptr;            // ptrs to each page of memory
  int *memusage;            // 0 if unused, else tag returned to requestor
                            // multiple pages marked same if requested together
                            // request may be smaller than entire alloc
  int *memcount;            // # of pages alloced starting with this page
                            // 0 if in middle of a contiguous alloc
  int npage;                // total # of pages currently allocated
  int npagemax;             // hi-water mark for # of pages allocated
  int tagmax;               // highest tag used thus far

  // alignment info

  int twolenbytes;          // byte length of two ints
  int kalign,valign;        // finalized alignments for keys/values
  int talign;               // alignment of entire KV or KMV pair
  int kalignm1,valignm1;    // alignments-1 for masking
  int talignm1;

  // file info

  uint64_t fsize;           // current aggregate size of disk files
  uint64_t fsizemax;        // hi-water mark for fsize

  int fcounter_kv;          // file counters for various intermediate files
  int fcounter_kmv;
  int fcounter_sort;
  int fcounter_part;
  int fcounter_set;

  // sorting

  typedef int (CompareFunc)(char *, int, char *, int);
  CompareFunc *compare;

  char **dptr;              // ptrs to datums being sorted
  int *slength;             // length of each datum being sorted

  // multi-block KMV info

  int kmv_block_valid;        // 1 if user is processing a multi-block KMV pair
  int kmv_key_page;           // which page the key info is on
  int kmv_nblock;             // # of value pages in multi-block KMV
  uint64_t kmv_nvalue_total;  // total # of values in multi-block KMV
  char *kmv_mvpage1;          // page ptrs for 2 multi-block blocks of values
  char *kmv_mvpage2;

  // file map()

  typedef void (MapFileFunc)(int, char *, int, class KeyValue *, void *);

  struct FileMap {
    int sepwhich;
    char sepchar;
    char *sepstr;
    int delta;
    char **filename;          // names of files to read
    uint64_t *filesize;       // size in bytes of each file
    int *tasksperfile;        // # of map tasks for each file
    int *whichfile;           // which file each map task reads
    int *whichtask;           // which sub-task in file each map task is
    MapFileFunc *appmapfile;  // user map function
    void *appptr;             // user data ptr
  };
  FileMap filemap;

  // private functions

  void defaults();
  void copy_kv(KeyValue *);
  void copy_kmv(KeyMultiValue *);

  uint64_t map_tasks(int, char **,
		     void (*)(int, KeyValue *, void *),
		     void (*)(int, char *, KeyValue *, void *),
		     void *, int, int);
  uint64_t map_chunks(int, int, char **,
		      void (*)(int, char *, int, class KeyValue *, void *),
		      void *, int);
  void findfiles(char *, int, int, int &, int &, char **&);
  void addfiles(char *, int, int &, int &, char **&);
  void bcastfiles(int &, char **&);

  void sort_kv(int);
  void sort_onepage(int, int, char *, char *, char *);
  void merge(int, int, void *, int, void *, int, void *);
  int extract(int, char *, char *&, int &);

  void stats(const char *, int);
  char *file_create(int);
  void file_stats(int);
  uint64_t roundup(uint64_t, int);
  void start_timer();
  void write_histo(double, const char *);
  void histogram(int, double *, double &, double &, double &,
		 int, int *, int *);

  void mr_stats(int);
  void allocate();
  void allocate_page(int);
  char *mem_request(int, uint64_t &, int &);
  void mem_unmark(int);
  void mem_cleanup();
  int mem_query(int &, int &);
  void memory_debug(int);
  void hiwater(int, uint64_t);
};

}

#endif
