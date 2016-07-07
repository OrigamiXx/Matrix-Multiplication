#!/usr/local/bin/python

# Make.py to create style_*.h files by parsing other files
# Syntax: Make.py

import sys,os,glob,commands,re

# style_command.h

files = glob.glob("*.h")
files.sort()

fp = open("style_command.tmp","w")
for file in files:
  txt = open(file,"r").read()
  if "COMMAND_CLASS" in txt:
    print >>fp,'#include "%s"' % file
fp.close()

if os.path.exists("style_command.h"):
  diff = commands.getoutput("diff style_command.h style_command.tmp")
else: diff = 1
if diff: os.rename("style_command.tmp","style_command.h")
else: os.remove("style_command.tmp")

# style_compare.h

files = glob.glob("compare_*.cpp")
files.sort()
hitlist = []

fp = open("style_compare.tmp","w")
print >>fp,"#ifdef COMPARE_STYLE\n"

pattern = re.compile("int \S+?\s*?\([^,\)]+?,[^,\)]+?," + 
                     "[^,\)]+?,[^,\)]+?\)",re.DOTALL)

for file in files:
  txt = open(file,"r").read()
  hits = re.findall(pattern,txt)
  hitlist += hits
  for hit in hits:
    patternword = "int (\S+?)\s*?\("
    funcname = re.findall(patternword,hit)
    print >>fp,"CompareStyle(%s)" % funcname[0]

print >>fp,"\n#else\n"

for hit in hitlist:
  print >>fp,"%s;" % hit

print >>fp,"\n#endif"

fp.close()

if os.path.exists("style_compare.h"):
  diff = commands.getoutput("diff style_compare.h style_compare.tmp")
else: diff = 1
if diff: os.rename("style_compare.tmp","style_compare.h")
else: os.remove("style_compare.tmp")

# style_hash.h

files = glob.glob("hash_*.cpp")
files.sort()
hitlist = []

fp = open("style_hash.tmp","w")
print >>fp,"#ifdef HASH_STYLE\n"

pattern = re.compile("int \S+?\s*?\([^,\)]+?,[^,\)]+?\)",re.DOTALL)

for file in files:
  txt = open(file,"r").read()
  hits = re.findall(pattern,txt)
  hitlist += hits
  for hit in hits:
    patternword = "int (\S+?)\s*?\("
    funcname = re.findall(patternword,hit)
    print >>fp,"HashStyle(%s)" % funcname[0]

print >>fp,"\n#else\n"

for hit in hitlist:
  print >>fp,"%s;" % hit

print >>fp,"\n#endif"

fp.close()

if os.path.exists("style_hash.h"):
  diff = commands.getoutput("diff style_hash.h style_hash.tmp")
else: diff = 1
if diff: os.rename("style_hash.tmp","style_hash.h")
else: os.remove("style_hash.tmp")

# style_map.h

files = glob.glob("map_*.cpp")
files.sort()
hitlist = []

fp = open("style_map.tmp","w")
print >>fp,"#if defined MAP_TASK_STYLE\n"

pattern = re.compile("void \S+?\s*?\([^,\)]+?,[^,\)]+?,[^,\)]+?\)",re.DOTALL)

for file in files:
  txt = open(file,"r").read()
  hits = re.findall(pattern,txt)
  hitlist += hits
  for hit in hits:
    patternword = "void (\S+?)\s*?\("
    funcname = re.findall(patternword,hit)
    print >>fp,"MapStyle(%s)" % funcname[0]

print >>fp,"\n#elif defined MAP_FILE_STYLE\n"

pattern = re.compile("void \S+?\s*?\([^,\)]+?,[^,\)]+?,[^,\)]+?," + 
                     "[^,\)]+?\)",re.DOTALL)

for file in files:
  txt = open(file,"r").read()
  hits = re.findall(pattern,txt)
  hitlist += hits
  for hit in hits:
    patternword = "void (\S+?)\s*?\("
    funcname = re.findall(patternword,hit)
    print >>fp,"MapStyle(%s)" % funcname[0]

print >>fp,"\n#elif defined MAP_STRING_STYLE\n"

pattern = re.compile("void \S+?\s*?\([^,\)]+?,[^,\)]+?,[^,\)]+?," + 
                     "[^,\)]+?,[^,\)]+?\)",re.DOTALL)

for file in files:
  txt = open(file,"r").read()
  hits = re.findall(pattern,txt)
  hitlist += hits
  for hit in hits:
    patternword = "void (\S+?)\s*?\("
    funcname = re.findall(patternword,hit)
    print >>fp,"MapStyle(%s)" % funcname[0]

print >>fp,"\n#elif defined MAP_MR_STYLE\n"

pattern = re.compile("void \S+?\s*?\([^,\)]+?,[^,\)]+?,[^,\)]+?," + 
                     "[^,\)]+?,[^,\)]+?,[^,\)]+?,[^,\)]+?\)",re.DOTALL)

for file in files:
  txt = open(file,"r").read()
  hits = re.findall(pattern,txt)
  hitlist += hits
  for hit in hits:
    patternword = "void (\S+?)\s*?\("
    funcname = re.findall(patternword,hit)
    print >>fp,"MapStyle(%s)" % funcname[0]

print >>fp,"\n#else\n"
print >>fp,'#include "mapreduce.h"'
print >>fp,"using MAPREDUCE_NS::MapReduce;"
print >>fp,"using MAPREDUCE_NS::KeyValue;\n"

for hit in hitlist:
  print >>fp,"%s;" % hit

print >>fp,"\n#endif"

fp.close()

if os.path.exists("style_map.h"):
  diff = commands.getoutput("diff style_map.h style_map.tmp")
else: diff = 1
if diff: os.rename("style_map.tmp","style_map.h")
else: os.remove("style_map.tmp")

# style_reduce.h

files = glob.glob("reduce_*.cpp")
files.sort()
hitlist = []

fp = open("style_reduce.tmp","w")
print >>fp,"#ifdef REDUCE_STYLE\n"

pattern = re.compile("void \S+?\s*?\([^,\)]+?,[^,\)]+?,[^,\)]+?,"
                     "[^,\)]+?,[^,\)]+?,[^,\)]+?,[^,\)]+?\)",re.DOTALL)

for file in files:
  txt = open(file,"r").read()
  hits = re.findall(pattern,txt)
  hitlist += hits
  for hit in hits:
    patternword = "void (\S+?)\s*?\("
    funcname = re.findall(patternword,hit)
    print >>fp,"ReduceStyle(%s)" % funcname[0]

print >>fp,"\n#else\n"
print >>fp,'#include "keyvalue.h"'
print >>fp,"using MAPREDUCE_NS::KeyValue;\n"

for hit in hitlist:
  print >>fp,"%s;" % hit

print >>fp,"\n#endif"

fp.close()

if os.path.exists("style_reduce.h"):
  diff = commands.getoutput("diff style_reduce.h style_reduce.tmp")
else: diff = 1
if diff: os.rename("style_reduce.tmp","style_reduce.h")
else: os.remove("style_reduce.tmp")

# style_scan.h

files = glob.glob("scan_*.cpp")
files.sort()
hitlist = []

fp = open("style_scan.tmp","w")
print >>fp,"#if defined SCAN_KV_STYLE\n"

pattern = re.compile("void \S+?\s*?\([^,\)]+?,[^,\)]+?,[^,\)]+?," +
                     "[^,\)]+?,[^,\)]+?\)",re.DOTALL)

for file in files:
  txt = open(file,"r").read()
  hits = re.findall(pattern,txt)
  hitlist += hits
  for hit in hits:
    patternword = "void (\S+?)\s*?\("
    funcname = re.findall(patternword,hit)
    print >>fp,"ScanStyle(%s)" % funcname[0]

print >>fp,"\n#elif defined SCAN_KMV_STYLE\n"

pattern = re.compile("void \S+?\s*?\([^,\)]+?,[^,\)]+?,[^,\)]+?,[^,\)]+?"
                     ",[^,\)]+?,[^,\)]+?\)",re.DOTALL)

for file in files:
  txt = open(file,"r").read()
  hits = re.findall(pattern,txt)
  hitlist += hits
  for hit in hits:
    patternword = "void (\S+?)\s*?\("
    funcname = re.findall(patternword,hit)
    print >>fp,"ScanStyle(%s)" % funcname[0]

print >>fp,"\n#else\n"

for hit in hitlist:
  print >>fp,"%s;" % hit

print >>fp,"\n#endif"

fp.close()

if os.path.exists("style_scan.h"):
  diff = commands.getoutput("diff style_scan.h style_scan.tmp")
else: diff = 1
if diff: os.rename("style_scan.tmp","style_scan.h")
else: os.remove("style_scan.tmp")
