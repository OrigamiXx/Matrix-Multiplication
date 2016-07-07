# nebula = nebula cluster, mpiCC, OpenMPI

SHELL = /bin/sh

# ---------------------------------------------------------------------
# compiler/archive settings
# specify flags and libraries needed for your compiler and MPI installation

CC =		/opt/openmpi-gnu-1.3.2/bin/mpiCC
CCFLAGS =	-O -I../../src
SHFLAGS =	-fPIC
DEPFLAGS =	-M

ARCHIVE =	ar
ARFLAGS =	-rc
SHLIBFLAGS =	-shared
SHLIB =         

# ---------------------------------------------------------------------
# build rules and dependencies
# no need to edit this section

# Library targets

lib:	$(OBJ)
	$(ARCHIVE) $(ARFLAGS) $(EXE) $(OBJ)

shlib:	$(OBJ)
	$(CC) $(CCFLAGS) $(SHFLAGS) $(SHLIBFLAGS) -o $(EXE) $(OBJ) $(SHLIB)

# Compilation rules

%.o:%.cpp
	$(CC) $(CCFLAGS) $(SHFLAGS) -c $<

%.d:%.cpp
	$(CC) $(CCFLAGS) $(DEPFLAGS) $< > $@

# Individual dependencies

DEPENDS = $(OBJ:.o=.d)
include $(DEPENDS)
