#!/usr/local/bin/python

# Manaul.py
# add per-function documentation to Section_functions.txt in tabular HTML format
# extract the comment lines from OINK source files between C-style /* ... */
# invoked by PDFgen.sh when PDF of doc pages is created
# Syntax: Manual.py

import sys,os,glob,commands,re

# mtxt, etc = contents of source files

files = glob.glob("../oink/map_*.cpp")
files.sort()
mtxt = ""
for file in files: mtxt += open(file,"r").read()

files = glob.glob("../oink/reduce_*.cpp")
files.sort()
rtxt = ""
for file in files: rtxt += open(file,"r").read()

files = glob.glob("../oink/compare_*.cpp")
files.sort()
ctxt = ""
for file in files: ctxt += open(file,"r").read()

files = glob.glob("../oink/hash_*.cpp")
files.sort()
htxt = ""
for file in files: htxt += open(file,"r").read()

files = glob.glob("../oink/scan_*.cpp")
files.sort()
stxt = ""
for file in files: stxt += open(file,"r").read()

# mcomm, etc = comments in source files, with /* and */ lines

pattern = re.compile("(/\*.*?\*/)",re.DOTALL)
mcomm = re.findall(pattern,mtxt)
rcomm = re.findall(pattern,rtxt)
ccomm = re.findall(pattern,ctxt)
hcomm = re.findall(pattern,htxt)
scomm = re.findall(pattern,stxt)

# mpair, etc = comments in source files, without /* and */ lines and whitespace

mpair = []
for comm in mcomm:
  lines = comm.split("\n")
  lines = [line.strip() for line in lines]
  mpair.append([lines[1],lines[2:-1]])

rpair = []
for comm in rcomm:
  lines = comm.split("\n")
  lines = [line.strip() for line in lines]
  rpair.append([lines[1],lines[2:-1]])

cpair = []
for comm in ccomm:
  lines = comm.split("\n")
  lines = [line.strip() for line in lines]
  cpair.append([lines[1],lines[2:-1]])

hpair = []
for comm in hcomm:
  lines = comm.split("\n")
  lines = [line.strip() for line in lines]
  hpair.append([lines[1],lines[2:-1]])

spair = []
for comm in scomm:
  lines = comm.split("\n")
  lines = [line.strip() for line in lines]
  spair.append([lines[1],lines[2:-1]])

# re-create Section_functions.txt file below double :line location
# txt2html does not know how to create multiline table entries
# so write tabular HTML format directly with <TR> and <TD>

txt = open("Section_functions.txt","r").read()

separator = "\n:line\n:line\n"
halves = txt.split(separator)
half2 = ""

half2 += "\nMap() functions :link(3_1),h4\n\n"
half2 += '<DIV ALIGN=center><TABLE WIDTH="0%" BORDER=1>'
for pair in mpair: 
  half2 += "<TR>\n"
  half2 += "<TD>%s</TD>\n" % pair[0]
  half2 += "<TD>\n"
  for line in pair[1]:
    half2 += "%s<BR>\n" % line
  half2 += "</TD>\n"
  half2 += "</TR>\n"
half2 += "</TABLE></DIV>\n"
half2 += "\n:line\n"

half2 += "\nReduce() functions :link(3_1),h4\n\n"
half2 += '<DIV ALIGN=center><TABLE WIDTH="0%" BORDER=1>'
for pair in rpair: 
  half2 += "<TR>\n"
  half2 += "<TD>%s</TD>\n" % pair[0]
  half2 += "<TD>\n"
  for line in pair[1]:
    half2 += "%s<BR>\n" % line
  half2 += "</TD>\n"
  half2 += "</TR>\n"
half2 += "</TABLE></DIV>\n"
half2 += "\n:line\n"

half2 += "\nCompare() functions :link(3_1),h4\n\n"
half2 += '<DIV ALIGN=center><TABLE WIDTH="0%" BORDER=1>'
for pair in cpair: 
  half2 += "<TR>\n"
  half2 += "<TD>%s</TD>\n" % pair[0]
  half2 += "<TD>\n"
  for line in pair[1]:
    half2 += "%s<BR>\n" % line
  half2 += "</TD>\n"
  half2 += "</TR>\n"
half2 += "</TABLE></DIV>\n"
half2 += "\n:line\n"

half2 += "\nHash() functions :link(3_1),h4\n\n"
half2 += '<DIV ALIGN=center><TABLE WIDTH="0%" BORDER=1>'
for pair in hpair: 
  half2 += "<TR>\n"
  half2 += "<TD>%s</TD>\n" % pair[0]
  half2 += "<TD>\n"
  for line in pair[1]:
    half2 += "%s<BR>\n" % line
  half2 += "</TD>\n"
  half2 += "</TR>\n"
half2 += "</TABLE></DIV>\n"
half2 += "\n:line\n"

half2 += "\nScan() functions :link(3_1),h4\n\n"
half2 += '<DIV ALIGN=center><TABLE WIDTH="0%" BORDER=1>'
for pair in spair: 
  half2 += "<TR>\n"
  half2 += "<TD>%s</TD>\n" % pair[0]
  half2 += "<TD>\n"
  for line in pair[1]:
    half2 += "%s<BR>\n" % line
  half2 += "</TD>\n"
  half2 += "</TR>\n"
half2 += "</TABLE></DIV>\n"
half2 += "\n:line\n"

txt = halves[0] + separator + half2
open("Section_functions.txt","w").write(txt)
