/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "mrmpi.h"
#include "object.h"
#include "input.h"
#include "variable.h"
#include "style_compare.h"
#include "style_hash.h"
#include "style_map.h"
#include "style_reduce.h"
#include "style_scan.h"
#include "error.h"

#include "mapreduce.h"
#include "keyvalue.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

/* ---------------------------------------------------------------------- */

MRMPI::MRMPI(OINK *oink) : Pointers(oink) {}

/* ----------------------------------------------------------------------
   invoke a MR-MPI library method on a MR object stored in Object class
------------------------------------------------------------------------- */

void MRMPI::run(int index, int narg, char **arg)
{
  if (narg < 1) error->all("Illegal MapReduce object command");

  MapReduce *mr = obj->extract_mr(index);

  // execute one of suite of MR-MPI library methods

  char *command = arg[0];
  arg++;
  narg--;

  if (strcmp(command,"delete") == 0) {
    if (narg > 0) error->all("Illegal MR object delete command");
    obj->delete_mr(index);
    obj->cleanup();
  } else if (strcmp(command,"copy") == 0) {
    if (narg != 1) error->all("Illegal MR object copy command");
    int index2 = obj->find_mr(arg[0]);
    if (index2 > 0) error->all("MR object created by copy already exists");
    MapReduce *mr2 = mr->copy();
    obj->add_mr(arg[0],mr2);
  } else if (strcmp(command,"add") == 0) {
    if (narg != 1) error->all("Illegal MR object add command");
    int index2 = obj->find_mr(arg[0]);
    if (index2 < 0) error->all("MR object add comand MR object does not exist");
    MapReduce *mr2 = obj->extract_mr(index2);
    mr->add(mr2);
  } else if (strcmp(command,"aggregate") == 0) {
    if (narg != 1) error->all("Illegal MR object aggregate command");
    if (strcmp(arg[0],"NULL") == 0) mr->aggregate(NULL);
    else {
      HashFnPtr hash = hash_lookup(arg[0]);
      mr->aggregate(hash);
    }
  } else if (strcmp(command,"broadcast") == 0) {
    if (narg != 1) error->all("Illegal MR object broadcast command");
    int root = atoi(arg[0]);
    mr->broadcast(root);
  } else if (strcmp(command,"clone") == 0) {
    if (narg != 0) error->all("Illegal MR object clone command");
    mr->clone();
  } else if (strcmp(command,"close") == 0) {
    if (narg != 0) error->all("Illegal MR object close command");
    mr->close();
  } else if (strcmp(command,"collapse") == 0) {
    if (narg != 2) error->all("Illegal MR object collapse command");
    if (strcmp(arg[0],"int") == 0) {
      int ikey = atoi(arg[1]);
      mr->collapse((char *) &ikey,sizeof(int));
    } else if (strcmp(arg[0],"uint64") == 0) {
      uint64_t ikey = atoll(arg[1]);
      mr->collapse((char *) &ikey,sizeof(uint64_t));
    } else if (strcmp(arg[0],"double") == 0) {
      double dkey = atof(arg[1]);
      mr->collapse((char *) &dkey,sizeof(double));
    } else if (strcmp(arg[0],"str") == 0) {
      char *str = arg[1];
      mr->collapse(str,strlen(str)+1);
    }
  } else if (strcmp(command,"collate") == 0) {
    if (narg != 1) error->all("Illegal MR object collate command");
    if (strcmp(arg[0],"NULL") == 0) mr->collate(NULL);
    else {
      HashFnPtr hash = hash_lookup(arg[0]);
      mr->collate(hash);
    }
  } else if (strcmp(command,"compress") == 0) {
    if (narg != 1) error->all("Illegal MR object compress command");
    ReduceFnPtr reduce = reduce_lookup(arg[0]);
    mr->compress(reduce,NULL);
  } else if (strcmp(command,"convert") == 0) {
    if (narg != 0) error->all("Illegal MR object convert command");
    mr->convert();
  } else if (strcmp(command,"gather") == 0) {
    if (narg != 1) error->all("Illegal MR object gather command");
    int nprocs = atoi(arg[0]);
    mr->gather(nprocs);

  } else if (strcmp(command,"map/task") == 0) {
    if (narg < 2 || narg > 3) error->all("Illegal MR object map/task command");
    int nmap = atoi(arg[0]);
    MapTaskFnPtr map = map_task_lookup(arg[1]);
    int addflag = 0;
    if (narg == 3) addflag = 1;
    mr->map(nmap,map,NULL,addflag);
 
 } else if (strcmp(command,"map/file") == 0) {
    if (narg < 5 || narg > 6) error->all("Illegal MR object map/file command");

    int nstr = 1;
    char **strings = &arg[0];
    if (strstr(arg[0],"v_") == arg[0]) {
      int index = input->variable->find(&arg[0][2]);
      if (index < 0) error->all("MR object map command variable is unknown");
      if (input->variable->equalstyle(index))
	error->all("MR object command input is equal-style variable");
      nstr = input->variable->retrieve_count(index);
      strings = new char*[nstr];
      for (int i = 0; i < nstr; i++) {
	char *one = input->variable->retrieve_single(index,i);
	int n = strlen(one) + 1;
	strings[i] = new char[n];
	strcpy(strings[i],one);
      }
    }

    int self = atoi(arg[1]);
    int recurse = atoi(arg[2]);
    int readfile = atoi(arg[3]);
    MapFileFnPtr map = map_file_lookup(arg[4]);
    int addflag = 0;
    if (narg == 6) addflag = 1;
    mr->map(nstr,strings,self,recurse,readfile,map,NULL,addflag);

    if (strings != &arg[0]) {
      for (int i = 0; i < nstr; i++) delete [] strings[i];
      delete [] strings;
    }

  } else if (strcmp(command,"map/char") == 0) {
    if (narg < 7 || narg > 8) error->all("Illegal MR object map/char command");

    int nmap = atoi(arg[0]);

    int nstr = 1;
    char **strings = &arg[1];
    if (strstr(arg[1],"v_") == arg[1]) {
      int index = input->variable->find(&arg[1][2]);
      if (index < 0) error->all("MR object map command variable is unknown");
      if (input->variable->equalstyle(index))
	error->all("MR object command input is equal-style variable");
      nstr = input->variable->retrieve_count(index);
      strings = new char*[nstr];
      for (int i = 0; i < nstr; i++) {
	char *one = input->variable->retrieve_single(index,i);
	int n = strlen(one) + 1;
	strings[i] = new char[n];
	strcpy(strings[i],one);
      }
    }

    int recurse = atoi(arg[2]);
    int readfile = atoi(arg[3]);
    char sepchar = arg[4][0];
    int delta = atoi(arg[5]);
    MapStringFnPtr map = map_string_lookup(arg[6]);
    int addflag = 0;
    if (narg == 8) addflag = 1;
    mr->map(nmap,nstr,strings,recurse,readfile,sepchar,delta,map,NULL,addflag);

    if (strings != &arg[0]) {
      for (int i = 0; i < nstr; i++) delete [] strings[i];
      delete [] strings;
    }

  } else if (strcmp(command,"map/string") == 0) {
    if (narg < 7 || narg > 8) error->all("Illegal MR object map/string command");

    int nmap = atoi(arg[0]);

    int nstr = 1;
    char **strings = &arg[1];
    if (strstr(arg[1],"v_") == arg[1]) {
      int index = input->variable->find(&arg[1][2]);
      if (index < 0) error->all("MR object map command variable is unknown");
      if (input->variable->equalstyle(index))
	error->all("MR object command input is equal-style variable");
      nstr = input->variable->retrieve_count(index);
      strings = new char*[nstr];
      for (int i = 0; i < nstr; i++) {
	char *one = input->variable->retrieve_single(index,i);
	int n = strlen(one) + 1;
	strings[i] = new char[n];
	strcpy(strings[i],one);
      }
    }

    int recurse = atoi(arg[2]);
    int readfile = atoi(arg[3]);
    char *sepstr = arg[4];
    int delta = atoi(arg[5]);
    MapStringFnPtr map = map_string_lookup(arg[6]);
    int addflag = 0;
    if (narg == 8) addflag = 1;
    mr->map(nmap,nstr,strings,recurse,readfile,sepstr,delta,map,NULL,addflag);

    if (strings != &arg[0]) {
      for (int i = 0; i < nstr; i++) delete [] strings[i];
      delete [] strings;
    }

  } else if (strcmp(command,"map/mr") == 0) {
    if (narg < 2 || narg > 3) error->all("Illegal MR object map/mr command");
    int index2 = obj->find_mr(arg[0]);
    if (index2 < 0) error->all("MR object map command MR object does not exist");
    MapReduce *mr2 = obj->extract_mr(index2);
    MapMRFnPtr map = map_mr_lookup(arg[1]);
    int addflag = 0;
    if (narg == 3) addflag = 1;
    mr->map(mr2,map,NULL,addflag);

  } else if (strcmp(command,"open") == 0) {
    if (narg != 0) error->all("Illegal MR object open command");
    mr->open();
  } else if (strcmp(command,"print") == 0) {
    if (narg == 4) {
      int proc = atoi(arg[0]);
      int nstride = atoi(arg[1]);
      int kflag = atoi(arg[2]);
      int vflag = atoi(arg[3]);
      mr->print(proc,nstride,kflag,vflag);
    } else if (narg == 6) {
      char *file = arg[0];
      int fflag = atoi(arg[1]);
      int proc = atoi(arg[2]);
      int nstride = atoi(arg[3]);
      int kflag = atoi(arg[4]);
      int vflag = atoi(arg[5]);
      mr->print(file,fflag,proc,nstride,kflag,vflag);
    } else error->all("Illegal MR object print command");

  } else if (strcmp(command,"reduce") == 0) {
    if (narg != 1) error->all("Illegal MR object reduce command");
    ReduceFnPtr reduce = reduce_lookup(arg[0]);
    mr->reduce(reduce,NULL);

  } else if (strcmp(command,"scan/kv") == 0) {
    if (narg != 1) error->all("Illegal MR object scan/kv command");
    ScanKVFnPtr scan = scan_kv_lookup(arg[0]);
    mr->scan(scan,NULL);
  } else if (strcmp(command,"scan/kmv") == 0) {
    if (narg != 1) error->all("Illegal MR object scan/kmv command");
    ScanKMVFnPtr scan = scan_kmv_lookup(arg[0]);
    mr->scan(scan,NULL);

  } else if (strcmp(command,"scrunch") == 0) {
    if (narg != 3) error->all("Illegal MR object scrunch command");
    int nprocs = atoi(arg[0]);
    if (strcmp(arg[1],"int") == 0) {
      int ikey = atoi(arg[2]);
      mr->scrunch(nprocs,(char *) &ikey,sizeof(int));
    } else if (strcmp(arg[1],"uint64") == 0) {
      uint64_t ikey = atoll(arg[2]);
      mr->scrunch(nprocs,(char *) &ikey,sizeof(uint64_t));
    } else if (strcmp(arg[1],"double") == 0) {
      double dkey = atoi(arg[2]);
      mr->scrunch(nprocs,(char *) &dkey,sizeof(double));
    } else if (strcmp(arg[1],"str") == 0) {
      char *str = arg[2];
      mr->scrunch(nprocs,str,strlen(str)+1);
    }
  } else if (strcmp(command,"sort_keys") == 0) {
    if (narg != 1) error->all("Illegal MR object sort_keys command");
    if (isdigit(arg[0][0])) {
      int flag = atoi(arg[0]);
      mr->sort_keys(flag);
    } else {
      CompareFnPtr compare = compare_lookup(arg[0]);
      mr->sort_keys(compare);
    }
  } else if (strcmp(command,"sort_values") == 0) {
    if (narg != 1) error->all("Illegal MR object sort_values command");
    if (isdigit(arg[0][0])) {
      int flag = atoi(arg[0]);
      mr->sort_values(flag);
    } else {
      CompareFnPtr compare = compare_lookup(arg[0]);
      mr->sort_values(compare);
    }
  } else if (strcmp(command,"sort_multivalues") == 0) {
    if (narg != 1) error->all("Illegal MR object sotr_multivalues command");
    if (isdigit(arg[0][0])) {
      int flag = atoi(arg[0]);
      mr->sort_multivalues(flag);
    } else {
      CompareFnPtr compare = compare_lookup(arg[0]);
      mr->sort_multivalues(compare);
    }
  } else if (strcmp(command,"kv_stats") == 0) {
    if (narg != 1) error->all("Illegal MR object kv_stats command");
    int level = atoi(arg[0]);
    mr->kv_stats(level);
  } else if (strcmp(command,"kmv_stats") == 0) {
    if (narg != 1) error->all("Illegal MR object kmv_stats command");
    int level = atoi(arg[0]);
    mr->kmv_stats(level);
  } else if (strcmp(command,"cummulative_stats") == 0) {
    if (narg != 2) error->all("Illegal MR object cummulative stats command");
    int level = atoi(arg[0]);
    int reset = atoi(arg[1]);
    mr->cummulative_stats(level,reset);

  } else if (strcmp(command,"set") == 0) {
    if (narg != 2) error->all("Illegal MR object set command");
    if (strcmp(arg[1],"mapstyle") == 0) mr->mapstyle = atoi(arg[2]);
    else if (strcmp(arg[1],"all2all") == 0) mr->all2all = atoi(arg[2]);
    else if (strcmp(arg[1],"verbosity") == 0) mr->verbosity = atoi(arg[2]);
    else if (strcmp(arg[1],"timer") == 0) mr->timer = atoi(arg[2]);
    else if (strcmp(arg[1],"memsize") == 0) mr->memsize = atoi(arg[2]);
    else if (strcmp(arg[1],"minpage") == 0) mr->minpage = atoi(arg[2]);
    else if (strcmp(arg[1],"maxpage") == 0) mr->maxpage = atoi(arg[2]);
    else if (strcmp(arg[1],"freepage") == 0) mr->freepage = atoi(arg[2]);
    else if (strcmp(arg[1],"outofcore") == 0) mr->outofcore = atoi(arg[2]);
    else if (strcmp(arg[1],"zeropage") == 0) mr->zeropage = atoi(arg[2]);
    else if (strcmp(arg[1],"keyalign") == 0) mr->keyalign = atoi(arg[2]);
    else if (strcmp(arg[1],"valuealign") == 0) mr->valuealign = atoi(arg[2]);
    else if (strcmp(arg[1],"fpath") == 0) mr->set_fpath(arg[2]);
    else error->all("Illegal MR object set command");

  } else error->all("Illegal MR object command");
}

/* ----------------------------------------------------------------------
   lookup methods to match function names with function pointers
------------------------------------------------------------------------- */

HashFnPtr MRMPI::hash_lookup(char *str)
{
  if (0) return NULL;

#define HASH_STYLE
#define HashStyle(name) else if (strcmp(str,#name) == 0) return name;
#include "style_hash.h"
#undef HASH_STYLE

  return NULL;
}

/* ---------------------------------------------------------------------- */

CompareFnPtr MRMPI::compare_lookup(char *str)
{
  if (0) return NULL;

#define COMPARE_STYLE
#define CompareStyle(name) else if (strcmp(str,#name) == 0) return name;
#include "style_compare.h"
#undef COMPARE_STYLE

  return NULL;
}

/* ---------------------------------------------------------------------- */

MapTaskFnPtr MRMPI::map_task_lookup(char *str)
{
  if (0) return NULL;

#define MAP_TASK_STYLE
#define MapStyle(name) else if (strcmp(str,#name) == 0) return name;
#include "style_map.h"
#undef MAP_TASK_STYLE

  return NULL;
}

/* ---------------------------------------------------------------------- */

MapFileFnPtr MRMPI::map_file_lookup(char *str)
{
  if (0) return NULL;

#define MAP_FILE_STYLE
#define MapStyle(name) else if (strcmp(str,#name) == 0) return name;
#include "style_map.h"
#undef MAP_FILE_STYLE

  return NULL;
}

/* ---------------------------------------------------------------------- */

MapStringFnPtr MRMPI::map_string_lookup(char *str)
{
  if (0) return NULL;

#define MAP_STRING_STYLE
#define MapStyle(name) else if (strcmp(str,#name) == 0) return name;
#include "style_map.h"
#undef MAP_STRING_STYLE

  return NULL;
}

/* ---------------------------------------------------------------------- */

MapMRFnPtr MRMPI::map_mr_lookup(char *str)
{
  if (0) return NULL;

#define MAP_MR_STYLE
#define MapStyle(name) else if (strcmp(str,#name) == 0) return name;
#include "style_map.h"
#undef MAP_MR_STYLE

  return NULL;
}

/* ---------------------------------------------------------------------- */

ReduceFnPtr MRMPI::reduce_lookup(char *str)
{
  if (0) return NULL;

#define REDUCE_STYLE
#define ReduceStyle(name) else if (strcmp(str,#name) == 0) return name;
#include "style_reduce.h"
#undef REDUCE_STYLE

  return NULL;
}

/* ---------------------------------------------------------------------- */

ScanKVFnPtr MRMPI::scan_kv_lookup(char *str)
{
  if (0) return NULL;

#define SCAN_KV_STYLE
#define ScanStyle(name) else if (strcmp(str,#name) == 0) return name;
#include "style_scan.h"
#undef SCAN_KV_STYLE

  return NULL;
}

/* ---------------------------------------------------------------------- */

ScanKMVFnPtr MRMPI::scan_kmv_lookup(char *str)
{
  if (0) return NULL;

#define SCAN_KMV_STYLE
#define ScanStyle(name) else if (strcmp(str,#name) == 0) return name;
#include "style_scan.h"
#undef SCAN_KMV_STYLE

  return NULL;
}
