/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifndef OINK_OBJECT_H
#define OINK_OBJECT_H

#include "pointers.h"
#include "mapreduce.h"
using MAPREDUCE_NS::MapReduce;
using MAPREDUCE_NS::KeyValue;

namespace OINK_NS {

class Object : protected Pointers {
 public:
  Object(class OINK *);
  ~Object();

  // invoked by run() method in Command class

  MapReduce *create_mr();
  MapReduce *create_mr(int, int, int, int);
  MapReduce *copy_mr(MapReduce *);
  int permanent(MapReduce *);

  MapReduce *input(int);
  MapReduce *input(int,
		   void (*)(int, char *, KeyValue *, void *), void *);
  MapReduce *input(int,
		   void (*)(int, char *, int, KeyValue *, void *),
		   void *);
  MapReduce *input(int,
		  void (*)(int, char *, KeyValue *, void *),
		  void (*)(int, char *, int, KeyValue *, void *),
		  void *);

  void output(int, MapReduce *);
  void output(int, MapReduce *,
	      void (*)(char *, int, char *, int, void *),
	      void *, int disallow = 0);
  void output(int, MapReduce *,
	      void (*)(char *, int, char *, int, int *, void *),
	      void *, int disallow = 0);
  void output(int, MapReduce *,
	      void (*)(uint64_t, char *, int, char *, int, KeyValue *, void *),
	      void *, int disallow = 0);
  void output(int, MapReduce *,
	      void (*)(char *, int, char *, int, int *, KeyValue *, void *),
	      void *, int disallow = 0);
  void output(int, MapReduce *,
	      void (*)(char *, int, char *, int, void *),
	      void (*)(char *, int, char *, int, int *, void *),
	      void (*)(uint64_t, char *, int, char *, int, KeyValue *, void *),
	      void (*)(char *, int, char *, int, int *, KeyValue *, void *),
	      void *, int disallow = 0);

  void cleanup();

  // invoked internally by Command class

  void setup_inputs(int);
  void setup_outputs(int);
  void add_input(int, char *);
  void add_output(int, char *, char *);

  // invoked by copy command in run() method in MRMPI class

  void add_mr(char *, MapReduce *);

  // inovked directly by input script

  void add_mr(int, char **);
  void user_input(int, char **);
  void user_output(int, char **);
  void set(int, char **);

  // invoked by various methods

  int find_mr(char *);
  MapReduce *extract_mr(int);
  void delete_mr(int);

 private:
  int me,nprocs;

  struct MRwrap {      // wrapper on MapReduce object
    int mode;          // 0 = unnamed temporary MR, 1 = named permanent MR
    char *name;        // name given MR object by user, NULL for temporary
    MapReduce *mr;     // ptr to actual MapReduce library object
  };

  int nmr,maxmr;       // number of wrapped MR objects
  MRwrap **mrwrap;     // wrapped MR objects

  struct Global {      // global settings for all commands
    int verbosity;     // default setting for each MapReduce object created
    int timer;         // ditto
    int memsize;       // ditto
    int outofcore;     // ditto
    int minpage;       // ditto
    int maxpage;       // ditto
    int freepage;      // ditto
    int zeropage;      // ditto
    char *scratch;     // ditto
    char *prepend;     // str to prepend to dir/file paths for scratch/in/out
    int substitute;    // substitution rule on % for scratch/in/out paths
                       // 0 = proc ID, N = ID%N + 1
  };

  Global global;

  struct Input {       // single command Input descriptor
    int index;         // which input
    int mode;          // 0 = path, 1 = named MR object
    int mrindex;       // index of an existing MR object
    char *prepend;     // override global/local prepend just for this input
    int substitute;    // override global/local subst just for this input
    int pflag,suflag;  // 1 if input-specific value has been set, 0 if not
    int multi;         // N to generate N paths for each input, 0 if not
    int nstr;          // # of strings resulting from paths
    char **strings;    // final strings resulting from paths
    int mmode;         // map method: 0 = entire files, 1 = sepchar, 2 = sepstr
    int recurse;       // arg to map() method
    int self;          // ditto
    int readfile;      // ditto
    int nmap;          // ditto
    char sepchar;      // ditto
    char *sepstr;      // ditto
    int delta;         // ditto
  };

  int ninput;          // # of inputs for next command
  Input **inputs;      // input descriptors
  int nuserinput;      // # of user inputs for next command
  Input **userinputs;  // user input descriptors

  struct Output {      // single command Output descriptor
    int index;         // which output
    int mode;          // 0 = file, 1 = named MR object, 2 = both
    char *name;        // name of MR object
    char *prepend;     // override global/local prepend just for this input
    int substitute;    // override global/local subst just for this input
    int pflag,suflag;  // 1 if input-specific value has been set, 0 if not
    char *procfile;    // file this proc will open/write
  };

  int noutput;          // # of outputs for next command
  Output **outputs;     // output descriptors
  int nuseroutput;      // # of user outputs for next command
  Output **useroutputs; // user outputs descriptors

  MapReduce *allocate_mr();
  MapReduce *allocate_mr(int, int, int, int);

  void clear_input(int &, Input **&);
  void clear_output(int &, Output **&);
  Input *default_input();
  Output *default_output();

  void expandpath(char *, char *, char *, int, int, int);
  void createdir(int, char *);
};

}

#endif
