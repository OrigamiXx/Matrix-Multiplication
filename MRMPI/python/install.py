#!/usr/local/bin/python

# copy MR-MPI src/libmrmpi.so and mrmpi.py to system dirs

instructions = """
Syntax: python install.py [-h] [libdir] [pydir]
        libdir = target dir for src/libmrmpi.so, default = /usr/local/lib
        pydir = target dir for mrmpi.py, default = Python site-packages dir
"""

import sys,os,commands

if (len(sys.argv) > 1 and sys.argv[1] == "-h") or len(sys.argv) > 3:
  print instructions
  sys.exit()

if len(sys.argv) >= 2: libdir = sys.argv[1]
else: libdir = "/usr/local/lib"

if len(sys.argv) == 3: pydir = sys.argv[2]
else: pydir = ""

# copy C lib to libdir if it exists
# warn if not in LD_LIBRARY_PATH or LD_LIBRARY_PATH is undefined

if not os.path.isdir(libdir):
  print "ERROR: libdir %s does not exist" % libdir
  sys.exit()
  
if "LD_LIBRARY_PATH" not in os.environ:
  print "WARNING: LD_LIBRARY_PATH undefined, cannot check libdir %s" % libdir
else:
  libpaths = os.environ['LD_LIBRARY_PATH'].split(':')
  if libdir not in libpaths:
    print "WARNING: libdir %s not in LD_LIBRARY_PATH" % libdir

str = "cp ../src/libmrmpi.so %s" % libdir
print str
outstr = commands.getoutput(str)
if len(outstr.strip()): print outstr

# copy mrmpi.py to pydir if it exists
# if pydir not specified, install in site-packages via distutils setup()

if pydir:
  if not os.path.isdir(pydir):
    print "ERROR: pydir %s does not exist" % pydir
    sys.exit()
  str = "cp ../python/mrmpi.py %s" % pydir
  print str
  outstr = commands.getoutput(str)
  if len(outstr.strip()): print outstr
  sys.exit()
  
print "installing mrmpi.py in Python site-packages dir"

os.chdir('../python')                # in case invoked via make in src dir

from distutils.core import setup
sys.argv = ["setup.py","install"]    # as if had run "python setup.py install"
setup(name = "mrmpi",
      version = "15Aug12",
      author = "Steve Plimpton",
      author_email = "sjplimp@sandia.gov",
      url = "http://mapreduce.sandia.gov",
      description = "MR-MPI MapReduce library",
      py_modules = ["mrmpi"])
