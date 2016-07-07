/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "string.h"
#include "ctype.h"
#include "stdlib.h"
#include "sys/stat.h"
#include "object.h"
#include "input.h"
#include "variable.h"
#include "memory.h"
#include "error.h"

#include "mapreduce.h"

using namespace OINK_NS;
using namespace MAPREDUCE_NS;

#define DELTA 4
#define MAXLINE 1024
#define MBYTES 64

#define MIN(A,B) ((A) < (B)) ? (A) : (B)
#define MAX(A,B) ((A) > (B)) ? (A) : (B)

enum{PATH,MR,BOTH,NEITHER};
enum{TEMPORARY,PERMANENT,DELETE};

/* ---------------------------------------------------------------------- */

Object::Object(OINK *oink) : Pointers(oink)
{
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  nmr = maxmr = 0;
  mrwrap = NULL;

  ninput = noutput = 0;
  inputs = NULL;
  outputs = NULL;

  nuserinput = nuseroutput = 0;
  userinputs = NULL;
  useroutputs = NULL;

#ifdef MRMPI_MEMSIZE
  global.memsize = MRMPI_MEMSIZE;
#else
  global.memsize = MBYTES;
#endif

  global.verbosity = 0;
  global.timer = 0;
  global.outofcore = 0;
  global.minpage = 0;
  global.maxpage = 0;
  global.freepage = 1;
  global.zeropage = 0;
  global.scratch = NULL;
  global.prepend = NULL;
  global.substitute = 0;
}

/* ---------------------------------------------------------------------- */

Object::~Object()
{
  delete [] global.scratch;
  delete [] global.prepend;

  clear_input(ninput,inputs);
  clear_input(nuserinput,userinputs);
  clear_output(noutput,outputs);
  clear_output(nuseroutput,useroutputs);

  for (int i = 0; i < nmr; i++) {
    delete [] mrwrap[i]->name;
    delete mrwrap[i]->mr;
    delete mrwrap[i];
  }
  memory->sfree(mrwrap);
}

/* ----------------------------------------------------------------------
   create a MapReduce object with global settings
   add it to mrwrap as temporary MR object
   called by a command to create unnamed MR object
------------------------------------------------------------------------- */

MapReduce *Object::create_mr()
{
  MapReduce *mr = allocate_mr();
  add_mr(NULL,mr);
  return mr;
}

/* ----------------------------------------------------------------------
   create a MapReduce object with specified settings
   add it to mrwrap as temporary MR object
   called by a command to create unnamed MR object
------------------------------------------------------------------------- */

MapReduce *Object::create_mr(int verbosity, int timer,
			     int memsize, int outofcore)
{
  MapReduce *mr = allocate_mr(verbosity,timer,memsize,outofcore);
  add_mr(NULL,mr);
  return mr;
}

/* ----------------------------------------------------------------------
   copy a MapReduce object
   add new MapReduce object the copy creates to mrwrap as temporary MR object
   called by a command to create unnamed MR object that is copy of another
------------------------------------------------------------------------- */

MapReduce *Object::copy_mr(MapReduce *mr)
{
  MapReduce *mr2 = mr->copy();
  obj->add_mr(NULL,mr2);
  return mr2;
}

/* ----------------------------------------------------------------------
   return 0/1 if MapReduce object is in a temporary/permanent MR object
   errror if not in a MR object
------------------------------------------------------------------------- */

int Object::permanent(MapReduce *mr)
{
  for (int index = 0; index < nmr; index++)
    if (mrwrap[index]->mr == mr) return mrwrap[index]->mode;
  error->all("Object permanent() called for unknown MR object");
  return 0;
}

/* ----------------------------------------------------------------------
   variant of input(map1,map2) with neither map1 or map2 arg
------------------------------------------------------------------------- */

MapReduce *Object::input(int index)
{
  return input(index,NULL,NULL,NULL);
}

/* ----------------------------------------------------------------------
   variant of input(map1,map2) with only map1 arg
------------------------------------------------------------------------- */

MapReduce *Object::input(int index,
			 void (*map1)(int, char *, KeyValue *, void *),
			 void *ptr)
{
  return input(index,map1,NULL,ptr);
}

/* ----------------------------------------------------------------------
   variant of input(map1,map2) with only map2 arg
------------------------------------------------------------------------- */

MapReduce *Object::input(int index,
			 void (*map2)(int, char *, int, KeyValue *, void *),
			 void *ptr)
{
  return input(index,NULL,map2,ptr);
}

/* ----------------------------------------------------------------------
   process a user input from 1 to Ninput
   if input is a MR object, just return assocated MapReduce object
   if input is a file(s), then create MapReduce object and
     invoke map() method to populate it, and return it
   called at start of command's run() method on each -i arg
------------------------------------------------------------------------- */

MapReduce *Object::input(int index,
			 void (*map1)(int, char *, KeyValue *, void *),
			 void (*map2)(int, char *, int, KeyValue *, void *),
			 void *ptr)
{
  if (index < 1 || index > ninput)
    error->all("Command input invoked with invalid index");
  index--;
  Input *in = inputs[index];

  // user input is name of permanent MR object

  if (in->mode == MR) return mrwrap[in->mrindex]->mr;

  // user input is one or more file names
  // create a temporary MapReduce object and store it in MR list
  // populate it by reading from file(s) with appropriate map function

  MapReduce *mr = allocate_mr();
  add_mr(NULL,mr);

  if (map1 && map2) {
    if (in->mmode == 0)
      mr->map(in->nstr,in->strings,in->self,in->recurse,in->readfile,map1,ptr);
    else if (in->mmode == 1) 
      mr->map(in->nmap,in->nstr,in->strings,
	      in->recurse,in->readfile,in->sepchar,in->delta,map2,ptr);
    else if (in->mmode == 2) 
      mr->map(in->nmap,in->nstr,in->strings,
	      in->recurse,in->readfile,in->sepstr,in->delta,map2,ptr);

  } else if (map1) {
    if (in->mmode != 0)
      error->all("Command input map function does not match input mode");
    mr->map(in->nstr,in->strings,in->self,in->recurse,in->readfile,map1,ptr);

  } else if (map2) {
    if (in->mmode == 0)
      error->all("Comand input map function does not match input mode");
    if (in->mmode == 1)
      mr->map(in->nmap,in->nstr,in->strings,
	      in->recurse,in->readfile,in->sepchar,in->delta,map2,ptr);
    else if (in->mmode == 2)
      mr->map(in->nmap,in->nstr,in->strings,
	      in->recurse,in->readfile,in->sepstr,in->delta,map2,ptr);

  } else error->all("Command input not allowed from file");

  return mr;
}

/* ----------------------------------------------------------------------
   variant of output(map,reduce) with neither map or reduce arg
------------------------------------------------------------------------- */

void Object::output(int index, MapReduce *mr)
{
  output(index,mr,NULL,NULL,NULL,NULL,0);
}

/* ----------------------------------------------------------------------
   variant of output(scankv,scankmv,map,reduce) with scankv arg
------------------------------------------------------------------------- */

void Object::output(int index, MapReduce *mr,
		    void (*scankv)(char *, int, char *, int, void *), 
		    void *ptr, int disallow)
{
  output(index,mr,scankv,NULL,NULL,NULL,ptr,disallow);
}

/* ----------------------------------------------------------------------
   variant of output(scankv,scankmv,map,reduce) with scankmv arg
------------------------------------------------------------------------- */

void Object::output(int index, MapReduce *mr,
		    void (*scankmv)(char *, int, char *, int, int *, void *), 
		    void *ptr, int disallow)
{
  output(index,mr,NULL,scankmv,NULL,NULL,ptr,disallow);
}

/* ----------------------------------------------------------------------
   variant of output(scankv,scankmv,map,reduce) with map arg
------------------------------------------------------------------------- */

void Object::output(int index, MapReduce *mr,
		    void (*map)(uint64_t, char *, int, char *, int, 
				KeyValue *, void *), 
		    void *ptr, int disallow)
{
  output(index,mr,NULL,NULL,map,NULL,ptr,disallow);
}

/* ----------------------------------------------------------------------
   variant of output(scankv,scankmv,map,reduce) with reduce arg
------------------------------------------------------------------------- */

void Object::output(int index, MapReduce *mr,
		    void (*reduce)(char *, int, char *, int, int *, 
				   KeyValue *, void *), 
		    void *ptr, int disallow)
{
  output(index,mr,NULL,NULL,NULL,reduce,ptr,disallow);
}

/* ----------------------------------------------------------------------
   process a user output from 1 to Ninput
   if output is a MR object, assign permanent name to it, unless disallowed
   if output is a file, invoke one of 4 methods on it
   called at end of command's run() method on each -o arg
------------------------------------------------------------------------- */

void Object::output(int index, MapReduce *mr,
		    void (*scankv)(char *, int, char *, int, void *), 
		    void (*scankmv)(char *, int, char *, int, int *, void *),
		    void (*map)(uint64_t, char *, int, char *, int, 
				KeyValue *, void *), 
		    void (*reduce)(char *, int, char *, int, int *,
				   KeyValue *, void *),
		    void *ptr, int disallow)
{
  if (index < 1 || index > noutput)
    error->all("Command output invoked with invalid index");
  index--;
  Output *out = outputs[index];

  // assign permanent name to MR object that wraps MapReduce object
  // change any other MR object with same name to temporary

  if (out->mode == MR || out->mode == BOTH) {
    if (disallow) error->all("Command output as MR object not allowed");
    int index;
    for (index = 0; index < nmr; index++)
      if (mrwrap[index]->mr == mr) break;
    if (index == nmr) 
      error->all("Command output called with unknown MR object");
    delete [] mrwrap[index]->name;
    int n = strlen(out->name) + 1;
    mrwrap[index]->name = new char[n];
    strcpy(mrwrap[index]->name,out->name);
    mrwrap[index]->mode = PERMANENT;
    for (int i = 0; i < nmr; i++)
      if (i != index && mrwrap[i]->mode == PERMANENT &&
	  strcmp(mrwrap[i]->name,out->name) == 0) {
	delete [] mrwrap[i]->name;
	mrwrap[i]->name = NULL;
	mrwrap[i]->mode = TEMPORARY;
      }
  }

  // user output was file name
  // invoke the one non-NULL of 4 methods on MapReduce object
  // for map() method, need addflag=1 to prevent KV pairs being deleted

  if (out->mode == PATH || out->mode == BOTH) {
    FILE *fp = fopen(out->procfile,"w");
    if (fp == NULL) {
      char str[256];
      sprintf(str,"Command output could not open output file %s",
	      out->procfile);
      error->one(str);
    }
    if (ptr) {
      struct {
	FILE *fp;
	void *ptr;
      } two;
      two.fp = fp;
      two.ptr = ptr;
      if (scankv) mr->scan(scankv,&two);
      else if (scankmv) mr->scan(scankmv,&two);
      else if (map) mr->map(mr,map,&two,1);
      else if (reduce) mr->reduce(reduce,&two);
      else error->all("Command input not allowed to file");
    } else {
      if (scankv) mr->scan(scankv,fp);
      else if (scankmv) mr->scan(scankmv,fp);
      else if (map) mr->map(mr,map,fp,1);
      else if (reduce) mr->reduce(reduce,fp);
      else error->all("Command input not allowed to file");
    }
    fclose(fp);
  }
}

/* ----------------------------------------------------------------------
   called at end of command's run() method 
   called when an MR is explicitly deleted by delete command in input script
------------------------------------------------------------------------- */

void Object::cleanup()
{
  // delete temporary MRs and named MRs flagged with -1

  for (int i = 0; i < nmr; i++)
    if (mrwrap[i]->mode != PERMANENT) {
      delete mrwrap[i]->mr;
      delete [] mrwrap[i]->name;
      delete mrwrap[i];
      for (int j = i+1; j < nmr; j++)
	mrwrap[j-1] = mrwrap[j];
      nmr--;
    }

  // remove all input/output descriptors

  clear_input(ninput,inputs);
  clear_input(nuserinput,userinputs);
  clear_output(noutput,outputs);
  clear_output(nuseroutput,useroutputs);
}

/* ----------------------------------------------------------------------
   called when input script command is processed
------------------------------------------------------------------------- */

void Object::setup_inputs(int n)
{
  clear_input(ninput,inputs);
  inputs = (Input **) memory->smalloc(n*sizeof(Input *),"object:inputs");
  ninput = n;
}

/* ----------------------------------------------------------------------
   called when input script command is processed
------------------------------------------------------------------------- */

void Object::setup_outputs(int n)
{
  clear_output(noutput,outputs);
  outputs = (Output **) memory->smalloc(n*sizeof(Output *),"object:outputs");
  noutput = n;
}

/* ----------------------------------------------------------------------
   called by each -i arg in in input script command
------------------------------------------------------------------------- */

void Object::add_input(int index, char *str)
{
  // check if any user settings exist for this input
  // if so, use that input descriptor, else create a default one

  int iwhich;
  for (iwhich = 0; iwhich < nuserinput; iwhich++)
    if (userinputs[iwhich]->index == index) break;

  Input *in;
  if (iwhich == nuserinput) in = default_input();
  else {
    in = userinputs[iwhich];
    userinputs[iwhich] = NULL;
  }
  in->index = index;
  inputs[index] = in;

  // input is a named MR object

  int imr = find_mr(str);
  if (imr >= 0) {
    in->mode = MR;
    in->mrindex = imr;
    return;
  }

  // input is one or more files
  // convert str to nstr,strings
  // if str is v_name, then request one string at a time from variable
  // expandpath() is function of prepend,substitute,multi settings
  // have to use oink->input intead of input b/c Object class re-defines input

  in->mode = PATH;

  int ivar = -1;
  int n = 1;
  if (strstr(str,"v_") == str) {
    ivar = oink->input->variable->find(&str[2]);
    if (ivar < 0) error->all("Command input variable is unknown");
    if (oink->input->variable->equalstyle(ivar))
      error->all("Command input is equal-style variable");
    n = oink->input->variable->retrieve_count(ivar);
  }

  int nstr = 0;
  char **strings = new char*[n*in->multi];
  char *one = str;

  for (int i = 0; i < n; i++) {
    if (ivar >= 0) one = oink->input->variable->retrieve_single(ivar,i);
    for (int j = 0; j < in->multi; j++) {
      strings[nstr] = new char[MAXLINE];
      if (in->pflag && in->suflag)
	expandpath(one,strings[nstr],in->prepend,0,in->substitute,j+1);
      else if (in->pflag)
	expandpath(one,strings[nstr],in->prepend,0,global.substitute,j+1);
      else if (in->suflag)
	expandpath(one,strings[nstr],global.prepend,0,in->substitute,j+1);
      else
	expandpath(one,strings[nstr],global.prepend,0,global.substitute,j+1);
      nstr++;
    }
  }

  in->nstr = nstr;
  in->strings = strings;
}

/* ----------------------------------------------------------------------
   called by each -o arg in input script command
------------------------------------------------------------------------- */

void Object::add_output(int index, char *file, char *name)
{
  // check if any user settings exist for this output
  // if so, use that output descriptor, else create a default one

  int iwhich;
  for (iwhich = 0; iwhich < nuseroutput; iwhich++)
    if (useroutputs[iwhich]->index == index) break;

  Output *out;
  if (iwhich == nuseroutput) out = default_output();
  else {
    out = useroutputs[iwhich];
    useroutputs[iwhich] = NULL;
  }
  out->index = index;
  outputs[index] = out;

  // output is a named MR object

  if (strcmp(name,"NULL") != 0) {
    int n = strlen(name) + 1;
    for (int i = 0; i < n-1; i++)
      if (!isalnum(name[i]) && name[i] != '_')
	error->all("Ouptut MR ID must be alphanumeric or "
		   "underscore characters");
    out->name = new char[n];
    strcpy(out->name,name);
  }

  // output is a file
  // convert str to procfile
  // expandpath() is function of prepend,substitute settings
  // create dirs in path as needed via createdir()

  if (strcmp(file,"NULL") != 0) {
    out->procfile = new char[MAXLINE];
    if (out->pflag && out->suflag)
      expandpath(file,out->procfile,out->prepend,1,out->substitute,0);
    else if (out->pflag)
      expandpath(file,out->procfile,out->prepend,1,global.substitute,0);
    else if (out->suflag)
      expandpath(file,out->procfile,global.prepend,1,out->substitute,0);
    else
      expandpath(file,out->procfile,global.prepend,1,global.substitute,0);

    createdir(1,out->procfile);
  }

  // set output mode

  if (out->procfile && out->name) out->mode = BOTH;
  else if (out->procfile) out->mode = PATH;
  else if (out->name) out->mode = MR;
  else out->mode = NEITHER;
}

/* ----------------------------------------------------------------------
   add a MR object to MRwrap
   if name = NULL, add as temporary MR object
   else add as permanent MR object
   name assumed to not already exist (caller must check this)
   called with NULL by various other Object methods
   called with name by copy command in MRMPI::run() and via input script "mr" command
------------------------------------------------------------------------- */

void Object::add_mr(char *name, MapReduce *mr)
{
  if (nmr == maxmr) {
    maxmr += DELTA;
    mrwrap = (MRwrap **) memory->srealloc(mrwrap,maxmr*sizeof(MRwrap *),
					  "object:mrwrap");
  }

  mrwrap[nmr] = new MRwrap();
  if (name == NULL) {
    mrwrap[nmr]->mode = TEMPORARY;
    mrwrap[nmr]->name = NULL;
  } else {
    mrwrap[nmr]->mode = PERMANENT;
    int n = strlen(name) + 1;
    mrwrap[nmr]->name = new char[n];
    strcpy(mrwrap[nmr]->name,name);
  }
  mrwrap[nmr]->mr = mr;
  nmr++;
}

/* ----------------------------------------------------------------------
   allocate and add a named MR object to MRwrap
   additional args set MapReduce object settings
   called by "mr" command in input script
------------------------------------------------------------------------- */

void Object::add_mr(int narg, char **arg)
{
  if (narg < 1 || narg > 5) error->all("Illegal mr command");

  int n = strlen(arg[0]);
  for (int i = 0; i < n; i++)
    if (!isalnum(arg[0][i]) && arg[0][i] != '_')
      error->all("MR ID must be alphanumeric or underscore characters");

  if (find_mr(arg[0]) >= 0)
    error->all("ID in mr command is already in use");
  MapReduce *mr = allocate_mr();
  add_mr(arg[0],mr);

  if (narg >= 2) mr->verbosity = atoi(arg[1]);
  if (narg >= 3) mr->timer = atoi(arg[2]);
  if (narg >= 4) mr->memsize = atoi(arg[3]);
  if (narg == 5) mr->outofcore = atoi(arg[4]);
}

/* ----------------------------------------------------------------------
   called via "input" command in input script
------------------------------------------------------------------------- */

void Object::user_input(int narg, char **arg)
{
  if (narg < 3) error->all("Illegal input command");

  int index = atoi(arg[0]) - 1;

  int iwhich;
  for (iwhich = 0; iwhich < nuserinput; iwhich++)
    if (userinputs[iwhich]->index == index) break;
  if (iwhich == nuserinput) {
    nuserinput++;
    userinputs = (Input **) memory->srealloc(userinputs,
					     nuserinput*sizeof(Input *),
					     "object:userinputs");
    userinputs[iwhich] = default_input();
  }

  Input *in = userinputs[iwhich];
  in->index = index;

  int iarg = 1;
  while (iarg < narg) {
    if (iarg+2 > narg) error->all("Illegal input command");
    if (strcmp(arg[iarg],"prepend") == 0) {
      in->pflag = 1;
      delete [] in->prepend;
      int n = strlen(arg[iarg+1]) + 1;
      in->prepend = new char[n];
      strcpy(in->prepend,arg[iarg+1]);
    } else if (strcmp(arg[iarg],"substitute") == 0) {
      in->suflag = 1;
      in->substitute = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"multi") == 0) {
      in->multi = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"mmode") == 0) {
      in->mmode = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"recurse") == 0) {
      in->recurse = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"self") == 0) {
      in->self = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"readfile") == 0) {
      in->readfile = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"nmap") == 0) {
      in->nmap = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"sepchar") == 0) {
      in->sepchar = arg[iarg+1][0];
    } else if (strcmp(arg[iarg],"sepstr") == 0) {
      int n = strlen(arg[iarg+1]) + 1;
      in->sepstr = new char[n];
      strcpy(in->sepstr,arg[iarg+1]);
    } else if (strcmp(arg[iarg],"delta") == 0) {
      in->delta = atoi(arg[iarg+1]);
    } else error->all("Illegal input command");
    iarg += 2;
  }
}

/* ----------------------------------------------------------------------
   called via "output" command in input script
------------------------------------------------------------------------- */

void Object::user_output(int narg, char **arg)
{
  if (narg < 3) error->all("Illegal input command");

  int index = atoi(arg[0]) - 1;

  int iwhich;
  for (iwhich = 0; iwhich < nuseroutput; iwhich++)
    if (useroutputs[iwhich]->index == index) break;
  if (iwhich == nuseroutput) {
    nuseroutput++;
    useroutputs = (Output **) memory->srealloc(useroutputs,
					       nuseroutput*sizeof(Output *),
					       "object:useroutputs");
    useroutputs[iwhich] = default_output();
  }

  Output *out = useroutputs[iwhich];
  out->index = index;

  int iarg = 1;
  while (iarg < narg) {
    if (iarg+2 > narg) error->all("Illegal output command");
    if (strcmp(arg[iarg],"prepend") == 0) {
      out->pflag = 1;
      delete [] out->prepend;
      int n = strlen(arg[iarg+1]) + 1;
      out->prepend = new char[n];
      strcpy(out->prepend,arg[iarg+1]);
    } else if (strcmp(arg[iarg],"substitute") == 0) {
      out->suflag = 1;
      out->substitute = atoi(arg[iarg+1]);
    } else error->all("Illegal output command");
    iarg += 2;
  }
}

/* ----------------------------------------------------------------------
   called via "set" command in input script
------------------------------------------------------------------------- */

void Object::set(int narg, char **arg)
{
  int iarg = 0;
  while (iarg < narg) {
    if (iarg+2 > narg) error->all("Illegal set command");
    if (strcmp(arg[iarg],"verbosity") == 0) {
      global.verbosity = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"timer") == 0) {
      global.timer = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"memsize") == 0) {
      global.memsize = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"outofcore") == 0) {
      global.outofcore = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"minpage") == 0) {
      global.minpage = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"maxpage") == 0) {
      global.maxpage = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"freepage") == 0) {
      global.freepage = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"zeropage") == 0) {
      global.zeropage = atoi(arg[iarg+1]);
    } else if (strcmp(arg[iarg],"scratch") == 0) {
      delete [] global.scratch;
      int n = strlen(arg[iarg+1]) + 1;
      global.scratch = new char[n];
      strcpy(global.scratch,arg[iarg+1]);
    } else if (strcmp(arg[iarg],"prepend") == 0) {
      delete [] global.prepend;
      int n = strlen(arg[iarg+1]) + 1;
      global.prepend = new char[n];
      strcpy(global.prepend,arg[iarg+1]);
    } else if (strcmp(arg[iarg],"substitute") == 0) {
      global.substitute = atoi(arg[iarg+1]);
    } else error->all("Illegal set command");
    iarg += 2;
  }
}

/* ----------------------------------------------------------------------
   return index of a named MR object
   return -1 if not found
------------------------------------------------------------------------- */

int Object::find_mr(char *name)
{
  int i;
  for (i = 0; i < nmr; i++)
    if (mrwrap[i]->mode == PERMANENT && 
	strcmp(mrwrap[i]->name,name) == 0) return i;
  return -1;
}

/* ----------------------------------------------------------------------
   return MapReduce object pointer wrapped by MR object with index
------------------------------------------------------------------------- */

MapReduce *Object::extract_mr(int index)
{
  return mrwrap[index]->mr;
}

/* ----------------------------------------------------------------------
   mark a MR object for deletion
------------------------------------------------------------------------- */

void Object::delete_mr(int index)
{
  mrwrap[index]->mode = DELETE;
}

/* ----------------------------------------------------------------------
   allocate a MapReduce object with global settings
------------------------------------------------------------------------- */

MapReduce *Object::allocate_mr()
{
  return allocate_mr(global.verbosity,global.timer,
		     global.memsize,global.outofcore);
}

/* ----------------------------------------------------------------------
   allocate a MapReduce object with specified settings
------------------------------------------------------------------------- */

MapReduce *Object::allocate_mr(int verbosity, int timer,
			       int memsize, int outofcore)
{
  MapReduce *mr = new MapReduce(world);
  mr->verbosity = verbosity;
  mr->timer = timer;
  mr->memsize = memsize;
  mr->outofcore = outofcore;

  mr->minpage = global.minpage;
  mr->maxpage = global.maxpage;
  mr->freepage = global.freepage;
  mr->zeropage = global.zeropage;

  if (global.scratch) {
    char sdir[MAXLINE];
    expandpath(global.scratch,sdir,global.prepend,0,global.substitute,0);
    createdir(0,sdir);
    mr->set_fpath(sdir);
  }

  return mr;
}

/* ----------------------------------------------------------------------
   entry in array could be NULL   
------------------------------------------------------------------------- */

void Object::clear_input(int &n, Input **&in)
{
  for (int i = 0; i < n; i++)
    if (in[i]) {
      delete [] in[i]->prepend;
      for (int j = 0; j < in[i]->nstr; j++) delete [] in[i]->strings[j];
      delete [] in[i]->strings;
      delete [] in[i]->sepstr;
      delete in[i];
    }
  memory->sfree(in);
  n = 0;
  in = NULL;
}

/* ----------------------------------------------------------------------
   entry in array could be NULL   
------------------------------------------------------------------------- */

void Object::clear_output(int &n, Output **&out)
{
  for (int i = 0; i < n; i++)
    if (out[i]) {
      delete [] out[i]->name;
      delete [] out[i]->prepend;
      delete [] out[i]->procfile;
      delete out[i];
    }
  memory->sfree(out);
  n = 0;
  out = NULL;
}

/* ----------------------------------------------------------------------
   allocated an Input descriptor and set all its defaults and strings
------------------------------------------------------------------------- */

Object::Input *Object::default_input()
{
  Input *in = new Input();
  in->index = -1;
  in->prepend = NULL;
  in->pflag = 0;
  in->suflag = 0;
  in->multi = 1;
  in->nstr = 0;
  in->strings = NULL;
  in->mmode = 0;
  in->recurse = 0;
  in->self = 0;
  in->readfile = 0;
  in->nmap = 0;
  in->sepchar = '\n';
  in->sepstr = new char[2];
  strcpy(in->sepstr,"\n");
  in->delta = 80;
  return in;
}

/* ----------------------------------------------------------------------
   allocated an Output descriptor and set all its defaults and strings
------------------------------------------------------------------------- */

Object::Output *Object::default_output()
{
  Output *out = new Output();
  out->index = -1;
  out->name = NULL;
  out->prepend = NULL;
  out->pflag = 0;
  out->suflag = 0;
  out->procfile = NULL;
  return out;
}

/* ----------------------------------------------------------------------
   expand inpath to outpath
   if prepend is non-NULL, add prepend as preceding path
   if postpend is 1, add proc ID as trailing suffix
   if % appears in path, replace first instance with proc ID
     for substitute = 0, use proc ID directly
     for substitute = N, replace if with ID % N + 1
       can be useful for multi-disk systems on multicore nodes
   if * appears in path, replace first instance with multi
     can be useful for multi-disk systems on multicore nodes
------------------------------------------------------------------------- */

void Object::expandpath(char *inpath, char *outpath,
			char *prepend, int postpend, int substitute, int multi)
{
  char tmppath[MAXLINE];

  if (prepend && postpend)
    sprintf(tmppath,"%s/%s.%d",prepend,inpath,me);
  else if (prepend)
    sprintf(tmppath,"%s/%s",prepend,inpath);
  else if (postpend)
    sprintf(tmppath,"%s.%d",inpath,me);
  else
    strcpy(tmppath,inpath);

  char *ptr = strchr(tmppath,'%');
  if (!ptr) strcpy(outpath,tmppath);
  else if (substitute == 0) {
    *ptr = '\0';
    sprintf(outpath,"%s%d%s",tmppath,me,ptr+1);
  } else {
    *ptr = '\0';
    sprintf(outpath,"%s%d%s",tmppath,(me%substitute)+1,ptr+1);
  }

  ptr = strchr(outpath,'*');
  if (!ptr) return;
  *ptr = '\0';
  sprintf(tmppath,"%s%d%s",outpath,multi,ptr+1);
  strcpy(outpath,tmppath);
}

/* ----------------------------------------------------------------------
   create dir portion(s) of path, as needed
   flag = 0, path = dir only
   flag = 1, path = dir/filename
------------------------------------------------------------------------- */

void Object::createdir(int flag, char *path)
{
  struct stat buf;
  char dironly[MAXLINE],copy[MAXLINE];

  // trim file from path if necessary
  // if no dir in path, just return

  strcpy(dironly,path);
  if (flag) {
    char *ptr = strrchr(dironly,'/');
    if (ptr) *ptr = '\0';
    else return;
  }

  // dironly is only dirs
  // create entire tmppath, one dir at a time, using copy
  // mkdir() just returns if dir already exists
  
  strcpy(copy,dironly);
  char *ptr = strtok(copy,"/");
  while (ptr) {
    if (strlen(ptr)) {
      int offset = ptr+strlen(ptr)-copy;
      char savechar = dironly[offset];
      dironly[offset] = '\0';
      mkdir(dironly,0777);
      dironly[offset] = savechar;
    }
    ptr = strtok(NULL,"/");
  }
  mkdir(dironly,0777);

  // test if successful

  if (stat(dironly,&buf) || !S_ISDIR(buf.st_mode)) {
    char str[256];
    sprintf(str,"Could not create dir for file %s\n",path);
    error->one(str);
  }
}
